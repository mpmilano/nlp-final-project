#pragma once

struct FirstVector{
	const double commas_char; 
	const double commas_word;
	const double commas_sentence;
	const double periods_chars;
	const double periods_word;
	const double periods_sentence;
	const double quotes_chars;
	const double quotes_word;
	const double quotes_sentence;
	const double apostrophe_chars;
	const double apostrophe_word;
	const double apostrophe_sentence;
	const double ellipse_chars;
	const double ellipse_word;
	const double ellipse_sentence;
	const int numchars;
	const int numwords;
	const int numsent;
	const double percent_smallword_CAPS; //two-or-more characters
	const double percent_bigword_CAPS; //three-or-more characters
	const double help_star_disparity;
	const double help;
	const double stars;
	const int allpunct;
	const std::string &category;
	
	FirstVector(	double commas_char, 
			double commas_word,
			double commas_sentence,
			double periods_chars,
			double periods_word,
			double periods_sentence,
			double quotes_chars,
			double quotes_word,
			double quotes_sentence,
			double apostrophe_chars,
			double apostrophe_word,
			double apostrophe_sentence,
			double ellipse_chars,
			double ellipse_word,
			double ellipse_sentence,
			int numchars,
			int numwords,
			int numsent,
			double percent_smallword_CAPS, 
			double percent_bigword_CAPS,
			double help_star_disparity,
			double help,
			double stars, 
					int allpunct,	
					const std::string &category)
	:commas_char(commas_char), 
		commas_word(commas_word),
		commas_sentence(commas_sentence),
		periods_chars(periods_chars),
		periods_word(periods_word),
		periods_sentence(periods_sentence),
		quotes_chars(quotes_chars),
		quotes_word(quotes_word),
		quotes_sentence(quotes_sentence),
		apostrophe_chars(apostrophe_chars),
		apostrophe_word(apostrophe_word),
		apostrophe_sentence(apostrophe_sentence),
		ellipse_chars(ellipse_chars),
		ellipse_word(ellipse_word),
		ellipse_sentence(ellipse_sentence),
		numchars(numchars),
		numwords(numwords),
		numsent(numsent),
		percent_smallword_CAPS(percent_smallword_CAPS), 
		percent_bigword_CAPS(percent_bigword_CAPS),
		help_star_disparity(help_star_disparity),
		help(help),
		stars(stars),
	 allpunct(allpunct),
	 category(category){}

	friend std::ostream& operator<<(std::ostream&, const FirstVector& );
		
};

std::ostream& operator<<(std::ostream& os, const FirstVector& h){

	return (os << "<" << h.commas_char << "," <<
		h.commas_word << "," <<
		h.commas_sentence << "," << 
		h.periods_chars << "," << 
		h.periods_word << "," << 
		h.periods_sentence << "," << 
		h.quotes_chars << "," << 
		h.quotes_word << "," << 
		h.quotes_sentence << "," << 
		h.apostrophe_chars << "," << 
		h.apostrophe_word << "," << 
		h.apostrophe_sentence << "," << 
		h.ellipse_chars << "," << 
		h.ellipse_word << "," << 
		h.ellipse_sentence << "," << 
		h.numchars << "," << 
		h.numwords << "," << 
		h.numsent << "," << 
		h.percent_smallword_CAPS  << "," << 
		h.percent_bigword_CAPS << ",") << 
		h.help_star_disparity << "," << 
		h.help << "," << 
		h.stars << "," << 
		h.allpunct << "," << 
		h.category << ">" ;

}
