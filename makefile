main: user node
node: node.cpp md5percentile.cpp md5percentile.hpp
	g++ -o node node.cpp md5percentile.cpp
user: user.cpp
	g++ -o user user.cpp
clean:
	rm node
	rm user
