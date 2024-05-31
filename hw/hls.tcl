set CFLAGS "-Isrc -I/home/hartwigb/Desktop/PracticalLabs/6/util -Wno-unknown-pragmas" 	
set CSIMFLAGS "-Isrc -I/home/hartwigb/Desktop/PracticalLabs/6/util -Wno-unknown-pragmas" 
open_project simple
set_top hls_sobel
add_files /home/hartwigb/Desktop/PracticalLabs/6/hw/src/sobel.cpp 			-cflags ${CFLAGS} -csimflags ${CSIMFLAGS}
add_files /home/hartwigb/Desktop/PracticalLabs/6/hw/src/sobel.h 				-cflags ${CFLAGS} -csimflags ${CSIMFLAGS}
add_files -tb /home/hartwigb/Desktop/PracticalLabs/6/util/util.cpp 			-cflags ${CFLAGS} -csimflags ${CSIMFLAGS}
add_files -tb /home/hartwigb/Desktop/PracticalLabs/6/util/util.h 			-cflags ${CFLAGS} -csimflags ${CSIMFLAGS}
add_files -tb /home/hartwigb/Desktop/PracticalLabs/6/util/ref_sobel.cpp 	-cflags ${CFLAGS} -csimflags ${CSIMFLAGS}
add_files -tb /home/hartwigb/Desktop/PracticalLabs/6/util/ref_sobel.h 		-cflags ${CFLAGS} -csimflags ${CSIMFLAGS}
add_files -tb /home/hartwigb/Desktop/PracticalLabs/6/hw/src/tb_sobel.cpp 		-cflags ${CFLAGS} -csimflags ${CSIMFLAGS}
open_solution "solution1" -flow_target vitis
set_part  {xc7z020clg400-1}
create_clock -period 7
