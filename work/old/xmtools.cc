#define __STDC_LIMIT_MACROS
#include <Python.h>
#include "xmtools.h"

//
//  This function duplicates the functionality of Python's subprocess.call
//  Some versions of Python have a race condition which causes fork to silently
//  fail. We use this version instead.
//
static PyObject* xmtools_subproc(PyObject* self, PyObject* args, PyObject* kwds) {
    static const char* names[] = { "cmdlist", "stdin", "stdout", "stderr", 0 };
    PyObject* cmdlist = 0;
    const char* inpath = "/dev/null";
    const char* outpath = "/dev/null";
    const char* errpath = "/dev/null";

    if (!PyArg_ParseTupleAndKeywords(
        args, kwds, "O|sss", (char**)names,
        &cmdlist, &inpath, &outpath, &errpath
    )) return 0;

    if (!PySequence_Check(cmdlist)) {
        PyErr_SetString(PyExc_TypeError, "expected list of command line arguments");
        return 0;
    }

    Py_ssize_t len = PySequence_Length(cmdlist);
    if (len <= 0) {
        PyErr_SetString(PyExc_TypeError, "expected non-empty list of arguments");
        return 0;
    }

    char** cmdline = (char**)malloc((len+1)*sizeof(char*));
    xm::check(cmdline != 0, "malloc");

    for (Py_ssize_t ii = 0; ii<len; ii++) {
        PyObject* str = PySequence_GetItem(cmdlist, ii);
        if (!PyString_Check(str)) {
            Py_DECREF(str);
            for (Py_ssize_t jj = 0; jj<ii; jj++) free(cmdline[jj]);
            free(cmdline);
            PyErr_SetString(PyExc_TypeError, "expected list of command line arguments");
            return 0;
        }
        const char* ptr = PyString_AsString(str);
        xm::check(ptr != 0, "must be char ptr");
        cmdline[ii] = strdup(ptr);
        Py_DECREF(str);
    }
    cmdline[len] = 0;

    pid_t pid = fork();

    int code = 0;
    if (pid) { // parent
        PyThreadState *save = PyEval_SaveThread();
        xm::check(waitpid(pid, &code, 0) == pid, "sanity");
        PyEval_RestoreThread(save);
    } else { // child
        int fdin = open(inpath, O_RDONLY);
        int fdout = open(outpath, O_WRONLY | O_TRUNC | O_CREAT, 0666);
        int fderr = open(errpath, O_WRONLY | O_TRUNC | O_CREAT, 0666);
        close(0);
        close(1);
        close(2);
        dup2(fdin, 0);
        dup2(fdout, 1);
        dup2(fderr, 2);
        execv(cmdline[0], cmdline);
        xm::check(false, "shouldn't get here");
    }

    for (Py_ssize_t ii = 0; ii<len; ii++) {
        free(cmdline[ii]);
    }
    free(cmdline);

    return PyInt_FromLong(WIFEXITED(code) ? WEXITSTATUS(code) : -WTERMSIG(code));
}


static PyObject* xmtools_ephpath(PyObject* self, PyObject* args) {
    int msn = 0;
    char* ts = 0;
    if (!PyArg_ParseTuple(args, "is", &msn, &ts)) return 0;

    try {
        xm::timecode tc = xm::timecode::parse(ts);
        xm::str path = xm::ephpath(msn, getdate(tc));
        return Py_BuildValue("s", path.data());
    } catch (const std::exception& ee) {
        PyErr_SetString(PyExc_RuntimeError, ee.what());
    }

    return 0;
}

static PyMethodDef xmtools_functions[] = {
    { "subproc", (PyCFunction)xmtools_subproc, METH_VARARGS | METH_KEYWORDS,
        "subproc(['cmd', 'args', '...'], stdin='/dev/null', stdout='/dev/null', stderr='/dev/null') -> status" },
    { "ephpath", xmtools_ephpath, METH_VARARGS, "ephpath(msn, tc) -> path" },
    {0}
};

PyMODINIT_FUNC initxmtools (void) {
    PyObject* module = Py_InitModule3("xmtools", xmtools_functions,
        "Wrappers around C implementation in xmtools"
    );
    (void)module;
}
