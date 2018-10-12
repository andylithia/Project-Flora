#ifndef BCDUTIL_H_
#define BCDUTIL_H_

#define DEC2BCD(DEC) ((DEC % 10) + ((DEC / 10) % 10) * 16 )
#define BCD2DEC(PBCD) ((PBCD & 15) + ((PBCD >> 4) & 15) * 10 )
#define PBCDADD(A,B) DEC2BCD((BCD2DEC(A) + BCD2DEC(B)))
#define PBCDSUB(A,B) DEC2BCD((BCD2DEC(A) - BCD2DEC(B)))

#endif /* BCDUTIL_H_ */
