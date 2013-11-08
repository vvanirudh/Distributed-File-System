node: node.cpp md5percentile.cpp md5percentile.hpp
	g++ -o node node.cpp md5percentile.cpp
clean:
	rm node
