#pragma once

#include <limits.h>


enum RErrorType {

	R_NO_ERROR	=	0,

	// generic errors
	R_ERROR		=	INT_MIN,
	R_BAD_VALUE,
	R_UNSUPPORTED,
	R_NO_RESPONSE,
	R_NO_INIT,

};



class RError {
public:
								RError	(RErrorType = R_NO_ERROR,
											bool fatal = false);
								~RError	();

		const std::string&		ToString() const;
		const str::string&		ExtendedError() const;

		bool					IsFatal() const;

		RErrorType				Error() const;

		void					SetExtendedError(const std::string&);
		void					SetFatal(bool);
		void					SetError(RErrorType);

		bool	 				operator==(RErrorType) const;
		explicit				operator bool () const;
			// evaluates to true if we hold an error. C++11 safe-bool

		/*explicit*/			operator const std::string& () const;
			// returns ToString()

private:
		std::string				fErrorString,
								fExtendedError;

		RErrorType				fErrorType;
		bool					fFatal;
};


