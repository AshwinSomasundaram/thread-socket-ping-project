PROGS = a4pingClient a4pingServer alarm

all:	$(PROGS)

%:	%.cpp
	g++ $@.cpp -o $@ -pthread -std=c++11

clean:
	rm -f $(PROGS)
