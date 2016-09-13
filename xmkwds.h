
namespace xm {
    namespace internal {
        static void charescape(char c) {
            if (c == '\\') { printf("\\\\"); return; }
            if (c == '\"') { printf("\\\""); return; }
            if (c == '\n') { printf("\\n"); return; }
            if (c == '\r') { printf("\\r"); return; }
            if (c == '\t') { printf("\\t"); return; }
            printf("%c", c);
        }

        static void printkwd(const bluekeyword& kwd, const char* indent, bool last) {
            using namespace internal;

            const char* name = kwd.name.data();
            const void* data = kwd.bytes.data();
            const ssize_t len = kwd.bytes.size();
            const char code = kwd.code;
            ssize_t size = xmsuffix(code);

            printf("%s[\"%s\", ", indent, name);
            if (size == 0) {
                printf("\"%c\", \"", code);
                for (ssize_t ii = 0; ii<len; ii++) {
                    printf("\\%02x", ((unsigned char*)data)[ii]);
                }
                printf("\"%s\n", last ? "," : "");
                return;
            }
            ssize_t count = len/size;
            if (count == 1) {
                switch (code) {
                    case 'A':
                    case 'S':
                        printf("\"");
                        charescape(*(const char*)data);
                        printf("\"");
                        break;
                    // Signed Integers
                    case 'B': printf("%d", (int)*(const int8_t*)data); break;
                    case 'I': printf("%d", (int)*(const int16_t*)data); break;
                    case 'L': printf("%d", (int)*(const int32_t*)data); break;
                    case 'X': printf("%lld", *(const long long*)data); break;
                    // Floating Point
                    case 'F': printf("%.12le", (double)*(const float*)data); break;
                    case 'D': printf("%.18le", (double)*(const double*)data); break;
                    // Unsigned Integers
                    case 'O': printf("%u", (unsigned int)*(const uint8_t*)data); break;
                    case 'U': printf("%u", (unsigned int)*(const uint16_t*)data); break;
                    case 'V': printf("%u", (unsigned int)*(const uint32_t*)data); break;
                    default:
                        check(0, "unsupported type");
                        break;
                }
            } else {
                ssize_t ii;
                switch (code) {
                    case 'A': 
                    case 'S': 
                        printf("\"");
                        for (ii = 0; ii<count; ii++) charescape(((const char*)data)[ii]);
                        printf("\"");
                        break;

                    // Signed Integers
                    case 'B':
                        printf("[");
                        for (ii = 0; ii<count; ii++) {
                            printf("%d", (signed)((const int8_t*)data)[ii]);
                            if (ii != count - 1) printf(", ");
                        }
                        printf("]");
                        break;

                    case 'I':
                        printf("[");
                        for (ii = 0; ii<count; ii++) {
                            printf("%d", (signed)((const int16_t*)data)[ii]);
                            if (ii != count - 1) printf(", ");
                        }
                        printf("]");
                        break;

                    case 'L':
                        printf("[");
                        for (ii = 0; ii<count; ii++) {
                            printf("%d", (signed)((const int32_t*)data)[ii]);
                            if (ii != count - 1) printf(", ");
                        }
                        printf("]");
                        break;

                    case 'X':
                        printf("[");
                        for (ii = 0; ii<count; ii++) {
                            printf("%lld", ((const long long*)data)[ii]);
                            if (ii != count - 1) printf(", ");
                        }
                        printf("]");
                        break;

                    // Floating Point
                    case 'F':
                        printf("[");
                        for (ii = 0; ii<count; ii++) {
                            printf("%.12le", (double)((const float*)data)[ii]);
                            if (ii != count - 1) printf(", ");
                        }
                        printf("]");
                        break;

                    case 'D':
                        printf("[");
                        for (ii = 0; ii<count; ii++) {
                            printf("%.18le", (double)((const double*)data)[ii]);
                            if (ii != count - 1) printf(", ");
                        }
                        printf("]");
                        break;

                    // Unsigned Integers
                    case 'O':
                        printf("[");
                        for (ii = 0; ii<count; ii++) {
                            printf("%u", (unsigned)((const uint8_t*)data)[ii]);
                            if (ii != count - 1) printf(", ");
                        }
                        printf("]");
                        break;

                    case 'U':
                        printf("[");
                        for (ii = 0; ii<count; ii++) {
                            printf("%u", (unsigned)((const uint16_t*)data)[ii]);
                            if (ii != count - 1) printf(", ");
                        }
                        printf("]");
                        break;

                    case 'V':
                        printf("[");
                        for (ii = 0; ii<count; ii++) {
                            printf("%u", (unsigned)((const uint32_t*)data)[ii]);
                            if (ii != count - 1) printf(", ");
                        }
                        printf("]");
                        break;

                    default:
                        check(0, "unsupported type");
                        break;
                }
            }

            printf(last ? "]\n" : "],\n");
        }
    }
}

