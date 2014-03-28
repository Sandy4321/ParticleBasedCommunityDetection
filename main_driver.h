/*
 * main_driver.h
 *
 * Author: apetkova
 */


#ifndef MAIN_DRIVER__
#define MAIN_DRIVER__

#define FALSE 0
#define TRUE (!FALSE)

#define CHECK(err, errmsg) \
	if (err!=0) { \
		fprintf(stderr,"Error %d detected in line %d of file %s\n", \
			err,__LINE__,__FILE__); \
		fprintf(stderr, "%s\n", #errmsg); \
		return -1; \
	}

#endif  /* MAIN_DRIVER_ */




