all:
	g++-4.9.0 Main.cpp --std=c++1y -Wall -Wextra -g -o reviews -lboost_serialization -lboost_serialization-mt -lpthread -ldl -lutil -lm -lpython2.7 -Xlinker -export-dynamic

clang:
	clang++ Main.cpp -stdlib=libc++ --std=c++1y -Wall -Wextra -g -o reviews -ferror-limit=1 -lboost_serialization -lboost_serialization-mt -lpthread -ldl -lutil -lm -lpython2.7 -Xlinker -export-dynamic
