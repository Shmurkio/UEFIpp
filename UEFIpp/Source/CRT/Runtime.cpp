extern "C"
{
	void __chkstk()
	{
	}

	void _chkstk()
	{
	}

	int atexit(void (*)())
	{
		return 0;
	}

	int _fltused = 0;
}
