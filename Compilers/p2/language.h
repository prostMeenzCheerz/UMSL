#ifndef Language_H
#define Language_H

class Language {
	private:
		std::vector<char> alphabet;
		std::vector<std::string> keywords;
		std::vector<std::vector<std::string> > fsaTable;
	public:
		Language(); //class constructor - will instantiate Token object with alphabet, keywords, and fsa table
		void setAlphabet();
		void setKeywords();

		/**********************************************
		below I've used mutators to get the values from 
		the Token class private data members
		**********************************************/
		std::vector<char> getAlphabet() const 
			{ return alphabet; }
		std::vector<std::string> getKeywords() const 
			{ return keywords; }
};

#endif
