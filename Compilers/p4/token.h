#ifndef TOKEN_H
#define TOKEN_H

#include "language.h"

enum TokenID { idTK, numTK, equalTK, greaterTK, lessTK, colonTK, plusTK, minusTK, multiplyTK, slashTK, percentTK, periodTK, leftParanthesesTK, rightParanthesesTK, commaTK, leftBraceTK, rightBraceTK, 
	semi_colonTK, leftBracketTK, rightBracketTK, keyWordTK, EOFTK };

class Token {

	private:
		int inputFileLineNumber;
		std::string tokenInstance;
		TokenID enumInstance_tokenID;
	public:
		Token();
		void setInputFileLineNumber(int);
		void setTokenInstance(char);
		void setTokenID(int);
		void clearTokenInstance();

		TokenID getTokenID() const
			{ return enumInstance_tokenID; }
		int getInputFileLineNumber() const
			{ return inputFileLineNumber; }
		std::string getTokenInstance() const
			{ return tokenInstance; }

};

#endif
