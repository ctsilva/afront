// 2026: minimal binary I/O helpers for checkpoint files (raw native-endian PODs, vectors, sets, maps)
#ifndef _CKPT_IO_H
#define _CKPT_IO_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <set>
#include <map>
#include <vector>

class CkptOut {
public:
	explicit CkptOut(FILE *f) : f_(f) { }
	template <typename T> void pod(const T &x) { put(&x, sizeof(T)); }
	void put(const void *p, size_t n) { if (n && fwrite(p, 1, n, f_) != n) { perror("checkpoint write"); exit(2); } }
	void i64(long long x) { pod(x); }
	template <typename T> void podvec(const std::vector<T> &v) { i64(v.size()); put(v.data(), v.size()*sizeof(T)); }
	void ints(const std::set<int> &s) { i64(s.size()); for (std::set<int>::const_iterator i=s.begin(); i!=s.end(); ++i) pod(*i); }
	void tag(const char *t) { put(t, 4); }
private:
	FILE *f_;
};

class CkptIn {
public:
	explicit CkptIn(FILE *f) : f_(f) { }
	template <typename T> void pod(T &x) { get(&x, sizeof(T)); }
	void get(void *p, size_t n) { if (n && fread(p, 1, n, f_) != n) { fprintf(stderr, "checkpoint: truncated or unreadable file\n"); exit(2); } }
	long long i64() { long long x; pod(x); return x; }
	template <typename T> void podvec(std::vector<T> &v) { long long n = i64(); v.resize(n); get(v.data(), n*sizeof(T)); }
	void ints(std::set<int> &s) { s.clear(); long long n = i64(); for (long long i=0; i<n; i++) { int x; pod(x); s.insert(x); } }
	void tag(const char *t) { char b[4]; get(b, 4); if (b[0]!=t[0]||b[1]!=t[1]||b[2]!=t[2]||b[3]!=t[3]) { fprintf(stderr, "checkpoint: expected section '%.4s', found '%.4s'\n", t, b); exit(2); } }
private:
	FILE *f_;
};

#endif
