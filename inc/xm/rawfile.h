#ifndef XM_RAWFILE_H_
#define XM_RAWFILE_H_ 1

namespace xm {

    // XXX: This should change to: filedesc, fileptr, filemmap
    //{{{ internal
    namespace internal {

        // C++ doesn't provide a good way to work directly with file descriptors.
        // This class implements a container for the descriptor and a mmap ptr.
        //
        // This class returns errors rather than throwing exceptions, and we're
        // hiding it in an internal namespace for now.  Don't rely on it staying.
        //
        struct rawfile;
        static inline void swap(rawfile& flip, rawfile& flop);

        struct rawfile {
            ~rawfile() { clear(); }

            // default constructor or with file descriptor
            rawfile(int fd=-1) : fd(fd), ptr(0), len(0) {}

            /* XXX:
            // move constructor steals from the other object
            rawfile(rawfile&& other) : fd(other.fd), ptr(other.ptr), len(other.len) {
                other.fd = -1;
                other.ptr = 0;
                other.len = 0;
            }
            */

            /* XXX:
            rawfile& operator =(rawfile&& other) {
                clear();
                fd  = other.fd; other.fd  = -1;
                ptr = other.ptr; other.ptr = 0;
                len = other.len; other.len = 0;
                return *this;
            }
            */


            inline bool read(void* data, int64 bytes);
            inline bool write(const void* data, int64 bytes);
            inline bool seek(int64 offset);
            inline bool skip(int64 bytes);
            inline int64 size() const;
            inline const void* mmap();

            inline bool isfile() const;

            private:
                // no copies or defaults
                void operator =(rawfile& other);// = delete;
                rawfile(const rawfile&);// = delete;

                void clear() {
                    if (ptr) {
                        msync(ptr, len, MS_SYNC);
                        munmap(ptr, len);
                    }
                    if (fd >= 0) close(fd);
                }
                friend void swap(rawfile& flip, rawfile& flop);

                // file descriptor
                int fd;
                // mmap fields
                void* ptr;
                int64 len;
        };

        bool rawfile::read(void* ptr, int64 len) {
            char* buf = (char*)ptr;
            int64 want = len;
            while (want) {
                int64 got = ::read(fd, buf, want);
                if (got <= 0) {
                    break;
                }
                want -= got;
                buf += got;
            }
            return want == 0;
        }

        bool rawfile::write(const void* ptr, int64 len) {
            const char* buf = (const char*)ptr;
            while (len) {
                int64 put = ::write(fd, buf, len);
                if (put < 0) {
                    break;
                }
                len -= put;
                buf += put;
            }
            return len == 0;
        }

        bool rawfile::seek(int64 offset) {
            off_t result = ::lseek(fd, offset, SEEK_SET);
            return result != (off_t)-1;
        }

        bool rawfile::skip(int64 bytes) {
            if (bytes == 0) {
                return true;
            }
            off_t result = ::lseek(fd, bytes, SEEK_CUR);
            if (result == (off_t)-1) {
                if (errno != ESPIPE) {
                    return false;
                }
                // Can't quickly seek, so we'll manually dump data
                static char ignored[8192];
                while (bytes) {
                    int64 amt = bytes;
                    if (amt > 8192) {
                        amt = 8192;
                    }
                    int64 got = ::read(fd, ignored, amt);
                    if (got != amt) {
                        return false;
                    }
                    bytes -= got;
                }
            }
            return true;
        }

        int64 rawfile::size() const {
            struct stat st;
            check(fstat(fd, &st) == 0, "fstat");
            return st.st_size;
        }

        const void* rawfile::mmap() {
            if (ptr) return ptr;
            len = size();
            return ptr = (char*)::mmap(0, len, PROT_READ, MAP_SHARED, fd, 0);
        }

        bool rawfile::isfile() const {
            struct stat st;
            check(fstat(fd, &st) == 0, "fstat");
            return st.st_mode & S_IFREG;
        }

        static inline void swap(rawfile& flip, rawfile& flop) {
            xm::swap(flip.fd, flop.fd);
            xm::swap(flip.ptr, flop.ptr);
            xm::swap(flip.len, flop.len);
        }
    }

    //}}}

}

#endif // XM_RAWFILE_H_

