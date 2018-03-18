#ifndef XM_CMDLINE_H_
#define XM_CMDLINE_H_ 1

#include "basics.h"
#include "string.h"
#include "cartesian.h"
#include "geodetic.h"
#include "list.h"

namespace xm {

    struct cmdline {
        inline ~cmdline();
        inline cmdline(int argc, char const * const * const argv, string doc);

        inline string         getstring(    string name, string    def, string doc);
        inline double         getdouble(    string name, double    def, string doc);
        inline int64          getint64(     string name, int64     def, string doc);
        inline timecode       gettimecode(  string name, timecode  def, string doc);
        inline cartesian      getcartesian( string name, cartesian def, string doc);
        inline geodetic       getgeodetic(  string name, geodetic  def, string doc);

        inline string         getstring(    string name, string doc);
        inline double         getdouble(    string name, string doc);
        inline int64          getint64(     string name, string doc);
        inline timecode       gettimecode(  string name, string doc);
        inline cartesian      getcartesian( string name, string doc);
        inline geodetic       getgeodetic(  string name, string doc);

        inline bool           getswitch(    string name, string doc);

        inline string         getinput(     string name, string doc);
        inline string         getoutput(    string name, string doc);
        inline list<string>   getinputs(    string name, string doc);

        inline void done();

        private:
            cmdline();// = delete;
            cmdline(const cmdline&);// = delete;
            void operator =(const cmdline&);// = delete;

            inline const char* find_argument(const string& name);

            __attribute__ ((format (scanf, 7, 8)))
            inline bool scan_argument(
                string name, bool required, const char* type,
                const string& doc, int count, const char* format, ...
            );

            list<const char*> args;
            bool switches_done;
            bool need_help;
            bool done_called;
    };

    cmdline::~cmdline() { check(done_called, "must call cmdline .done()"); }

    cmdline::cmdline(int argc, char const * const * const argv, string doc)
        : switches_done(false), need_help(false), done_called(false)
    {
        for (int ii = 1; ii<argc; ii++) {
            args.append(argv[ii]);
            if (::strcmp(argv[ii], "-h") == 0 || ::strcmp(argv[ii], "-help") == 0) {
                need_help = true;
            }
        }
        if (need_help) {
            fprintf(stderr, "\n");
            fprintf(stderr, "Usage: %s [arguments ...]\n", argv[0]);
            fprintf(stderr, "    %s\n\n", doc.data());
            fprintf(stderr, "Arguments:\n");
        }
    }

    string cmdline::getstring(string name, string def, string doc) {
        check(switches_done == false, "switches must be processed first");
        name = format("-%s", name.data()); //name = "-" + name;
        if (need_help) {
            fprintf(
                stderr, "    %s <string, default='%s'>\n\t%s\n",
                name.data(), def.data(), doc.data()
            );
            return "";
        }

        const char* val = find_argument(name);
        return val ? val : def;
    }

    double cmdline::getdouble(string name, double def, string doc) {
        double val = 0;
        doc = format("<double, default=%lg>\n\t%s", def, doc.data());
        return scan_argument(name, false, "double", doc, 1, " %lf ", &val) ? val : def;
    }

    int64 cmdline::getint64(string name, int64 def, string doc) {
        int64 val = 0;
        doc = format("<int64, default=%lld>\n\t%s", def, doc.data());
        return scan_argument(name, false, "int64", doc, 1, " %lld ", &val) ? val : def;
    }

    timecode cmdline::gettimecode(string name, timecode def, string doc) {
        check(switches_done == false, "switches must be processed first");
        name = format("-%s", name.data()); //name = "-" + name;
        if (need_help) {
            string ts = format(def, 12);
            fprintf(
                stderr, "    %s <timecode, default=%s>\n\t%s\n",
                name.data(), ts.data(), doc.data()
            );
            return timecode();
        }

        const char* val = find_argument(name);
        return val ? timecode::parse(val) : def;
    }

    cartesian cmdline::getcartesian(string name, cartesian def, string doc) {
        cartesian val = cartesian();
        doc = format("<cartesian, default=%lg,%lg,%lg>\n\t%s", def.x, def.y, def.z, doc.data());
        return scan_argument(
            name, false, "cartesian", doc, 3, " %lf , %lf , %lf ", &val.x, &val.y, &val.z
        ) ? val : def;
    }

    geodetic cmdline::getgeodetic(string name, geodetic def, string doc) {
        geodetic val = geodetic();
        doc = format("<geodetic, default=%lg,%lg,%lg>\n\t%s", def.lat, def.lon, def.alt, doc.data());
        return scan_argument(
            name, false, "geodetic", doc, 3, " %lf , %lf , %lf ", &val.lat, &val.lon, &val.alt
        ) ? val : def;
    }

    string cmdline::getstring(string name, string doc) {
        check(switches_done == false, "switches must be processed first");
        name = format("-%s", name.data()); //name = "-" + name;
        if (need_help) {
            fprintf(
                stderr, "    %s <string, required>\n\t%s\n",
                name.data(), doc.data()
            );
            return "";
        }

        const char* val = find_argument(name);
        if (val == 0) {
            fprintf(stderr, "Error: switch '%s' is required\n", name.data());
            exit(-1);
        }
        return val;

    }

    double cmdline::getdouble(string name, string doc) {
        double val = 0;
        doc = format("<double, required>\n\t%s", doc.data());
        scan_argument(name, true, "double", doc, 1, " %lf ", &val);
        return val;
    }

    int64 cmdline::getint64(string name, string doc) {
        int64 val = 0;
        doc = format("<int64, required>\n\t%s", doc.data());
        scan_argument(name, true, "int64", doc, 1, " %lld ", &val);
        return val;
    }

    timecode cmdline::gettimecode(string name, string doc) {
        check(switches_done == false, "switches must be processed first");
        name = format("-%s", name.data()); //name = "-" + name;
        if (need_help) {
            fprintf(
                stderr, "    %s <timecode, required>\n\t%s\n",
                name.data(), doc.data()
            );
            return timecode();
        }

        const char* val = find_argument(name);
        if (!val) {
            fprintf(stderr, "Error: switch '%s' is required\n", name.data());
            exit(-1);
        }
        return timecode::parse(val);
    }

    cartesian cmdline::getcartesian(string name, string doc) {
        cartesian val = cartesian();
        doc = format("<cartesian, required>\n\t%s", doc.data());
        scan_argument(name, true, "cartesian", doc, 1, " %lf , %lf , %lf ", &val.x, &val.y, &val.z);
        return val;
    }

    geodetic cmdline::getgeodetic(string name, string doc) {
        geodetic val = geodetic();
        doc = format("<geodetic, required>\n\t%s", doc.data());
        scan_argument(name, true, "geodetic", doc, 1, " %lf , %lf , %lf ", &val.lat, &val.lon, &val.alt);
        return val;
    }

    bool cmdline::getswitch(string name, string doc) {
        check(switches_done == false, "switches must be processed first");
        name = format("-%s", name.data()); //name = "-" + name;
        if (need_help) {
            fprintf(stderr, "    %s\n\t%s\n", name.data(), doc.data());
            return false;
        }
        for (int64 ii = 0; ii<args.size(); ii++) {
            if (args[ii] && args[ii] == name) {
                args[ii] = 0;
                return true;
            }
        }
        return false;
    }

    string cmdline::getinput(string name, string doc) {
        switches_done = true;
        if (need_help) {
            fprintf(
                stderr, "    <%s, required input>\n\t%s\n",
                name.data(), doc.data()
            );
            return "";
        }
        for (int64 ii = 0; ii<args.size(); ii++) {
            if (args[ii]) {
                if (args[ii][0] == '-' && ::strcmp(args[ii], "-") != 0) {
                    fprintf(
                        stderr, "Error: found '%s' instead of input file name", args[ii]
                    );
                    exit(-1);
                }
                string result = args[ii];
                args[ii] = 0;
                return result;
            }
        }
        fprintf(
            stderr, "Error: expected an input file name\n"
            "Use -help for a complete list of arguments\n"
        );
        exit(-1);
        return "";
    }

    string cmdline::getoutput(string name, string doc) {
        switches_done = true;
        if (need_help) {
            fprintf(
                stderr, "    <%s, required output>\n\t%s\n",
                name.data(), doc.data()
            );
            return "";
        }
        for (int64 ii = 0; ii<args.size(); ii++) {
            if (args[ii]) {
                if (args[ii][0] == '-' && ::strcmp(args[ii], "-") != 0) {
                    fprintf(
                        stderr, "Error: found '%s' instead of output file name\n"
                        "Use -help for a complete list of arguments\n", args[ii]
                    );
                    exit(-1);
                }
                string result = args[ii];
                args[ii] = 0;
                return result;
            }
        }
        fprintf(
            stderr, "Error: expected an output file name\n"
            "Use -help for a complete list of arguments\n"
        );
        exit(-1);
        return "";
    }

    list<string> cmdline::getinputs(string name, string doc) {
        switches_done = true;
        if (need_help) {
            fprintf(
                stderr, "    <%s, multiple inputs> ...\n\t%s\n",
                name.data(), doc.data()
            );
            return list<string>();
        }
        list<string> results;
        for (int64 ii = 0; ii<args.size(); ii++) {
            if (args[ii]) {
                if (args[ii][0] == '-' && ::strcmp(args[ii], "-") != 0) {
                    fprintf(
                        stderr, "Error: found '%s' instead of input file name\n"
                        "Use -help for a complete list of arguments\n", args[ii]
                    );
                    exit(-1);
                }
                results.append(args[ii]);
                args[ii] = (char*)0;
            }
        }
        return results;
    }

    void cmdline::done() {
        bool die = false;

        if (need_help) {
            fprintf(stderr, "\n");
            exit(0);
        }

        for (int64 ii = 0; ii<args.size(); ii++) {
            if (args[ii] != 0) {
                fprintf(
                    stderr, "Error: Unrecognized or unused argument '%s'\n"
                    "Use -help for a complete list of arguments\n", args[ii]
                );
                die = true;
            }
        }

        if (die) exit(-1);
        done_called = true;
    }

    const char* cmdline::find_argument(const string& name) {
        for (int64 ii = 0; ii<args.size(); ii++) {
            if (args[ii] && args[ii] == name) {
                if (ii + 1 == args.size() || args[ii + 1] == 0) {
                    fprintf(
                        stderr, "switch '%s' requires an argument\n"
                        "Use -help for a complete list of arguments\n", name.data()
                    );
                    exit(-1);
                }
                const char* result = args[ii + 1];
                args[ii + 0] = (char*)0;
                args[ii + 1] = (char*)0;
                return result;
            }
        }
        return 0;
    }

    bool cmdline::scan_argument(
        string name, bool required, const char* type, const string& doc,
        int count, const char* format, ...
    ) {
        check(switches_done == false, "switches must be processed first");
        name = xm::format("-%s", name.data()); //name = "-" + name;

        if (need_help) {
            fprintf(stderr, "    %s %s\n", name.data(), doc.data());
            return false;
        }

        const char* val = find_argument(name);
        if (!val) {
            if (required) {
                fprintf(
                    stderr, "Error: %s switch %s is required\n"
                    "Use -help for a complete list of arguments\n", type, name.data()
                );
                exit(-1);
            }
            return false;
        }

        va_list ap;
        va_start(ap, format);
        int got = vsscanf(val, format, ap);
        va_end(ap);
        if (got < count) {
            fprintf(
                stderr, "Error: %s requires %d arguments\n"
                "Use -help for a complete list of arguments\n", name.data(), count
            );
            exit(-1);
        }

        return true;
    }

}

#endif // XM_CMDLINE_H_

