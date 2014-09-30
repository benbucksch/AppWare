#ifndef __SERUID_H__
#define __SERUID_H__

#define NULLSERUID	0

union SERUID
	{
	long int 	id;
	void far* ptr;

	int	operator ==(
		const SERUID far& uid2)
		{
		return (this->id == uid2.id);
		}

	int	operator !=(
		const SERUID far& uid2)
		{
		return (this->id != uid2.id);
		}

	SERUID(
		void)
		{
		}

	SERUID(
		long int	id)
		{
		this->id = id;
		}


	};

typedef SERUID far* LPSERUID;

#endif