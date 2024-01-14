# mongooseDemo1
Demonstration of mongoose.

Built with CMakeLists.txt.
Using vcpkg to get OpenSSL.
Tested on Ubuntu 22.04.3 on Raspberry Pi.
Tested on Windows 10 with Visual Studio Community 2022.

Command line options:
	1: testPost.  Post to https://httpbin.org to get small text back.
	2: testBigGet.  Get 8k response from https://httpbin.org.
	3: testManyPosts.   Call testPost 20 times.
