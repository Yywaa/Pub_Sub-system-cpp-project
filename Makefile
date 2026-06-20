CC =g++ -std=c++20
CFLAGS=-g -c

TARGET1=clientlib/libclient.a  #static library, compiled seperately
TARGET2=CoordinatorCore/libcoord.a
TARGET3=CoordinatorCore/coordinator.exe
TARGET4=pub_sub_examples/pub_skt_main.exe  #public socket exctuable 

TARGET: ${TARGET1} ${TARGET2} ${TARGET3} ${TARGET4}

#standard libs

STLIBS=-lpthread

COORDINATOR_OBJS=CoordinatorCore/coord_start.o\
				CoordinatorCore/coordDB.o	\
				CoordinatorCore/coord_msg.o		\
				Common/cmsgOp.o					\

CLIENTLIBS=-Lclientlib -lclient
${TARGET1}:clientlib/client.o Common/cmsgOp.o
	@echo "Building client library"
	ar rcs ${TARGET1} clientlib/client.o Common/cmsgOp.o
${TARGET2}:${COORDINATOR_OBJS}
	@echo "Build coordinator library"
	ar rcs ${TARGET2} ${COORDINATOR_OBJS}

${TARGET3}:CoordinatorCore/coord_main.o ${COORDINATOR_OBJS}
	@echo "Building cooordinator executable"
	${CC} -g CoordinatorCore/coord_main.o ${COORDINATOR_OBJS} -o ${TARGET3} ${STLIBS}

${TARGET4}:pub_sub_examples/pub_skt_main.o pub_sub_examples/pub_skt_example.o ${TARGET1}
	@echo "Building publisher socket executable"
	${CC} -g pub_sub_examples/pub_skt_main.o pub_sub_examples/pub_skt_example.o -o ${TARGET4} ${CLIENTLIBS}

CoordinatorCore/coord_main.o:CoordinatorCore/coord_main.cpp
	${CC} ${CFLAGS} CoordinatorCore/coord_main.cpp -o CoordinatorCore/coord_main.o

CoordinatorCore/coord_start.o: CoordinatorCore/coord_start.cpp
	${CC} ${CFLAGS} CoordinatorCore/coord_start.cpp -o CoordinatorCore/coord_start.o

CoordinatorCore/coordDB.o:CoordinatorCore/coordDB.cpp
	${CC} ${CFLAGS} CoordinatorCore/coordDB.cpp -o CoordinatorCore/coordDB.o
CoordinatorCore/coord_msg.o:CoordinatorCore/coord_msg.cpp
	${CC} ${CFLAGS} CoordinatorCore/coord_msg.cpp -o CoordinatorCore/coord_msg.o
Common/cmsgOp.o:Common/cmsgOp.cpp
	${CC} ${CFLAGS} Common/cmsgOp.cpp -o Common/cmsgOp.o
pub_sub_examples/pub_skt_main.o: pub_sub_examples/pub_skt_main.cpp
	${CC} ${CFLAGS} pub_sub_examples/pub_skt_main.cpp -o pub_sub_examples/pub_skt_main.o
pub_sub_examples/pub_skt_example.o: pub_sub_examples/pub_skt_example.cpp
	${CC} ${CFLAGS} pub_sub_examples/pub_skt_example.cpp -o pub_sub_examples/pub_skt_example.o

clean:
	rm -f *.a
	rm -f *.exe
	rm -f *.o
	rm -f CoordinatorCore/*.o
	rm -f CoordinatorCore/*.a
	rm -f CoordinatorCore/*.exe
	rm -f Common/*.o
	rm -f Common/*.a
	rm -f clientlib/*.o
	rm -f clientlib/*.a
	rm -f clientlib/*.exe
	rm -f pub_sub_examples/*.a
	rm -f pub_sub_examples/*.o
	rm -f pub_sub_examples/*.exe
	
	