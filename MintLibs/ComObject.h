#pragma once
namespace MintChoco
{
	class cComLibs {
	public:

		static bool		 Init();
		static void		 Deinit();
		static cComLibs* GetInstance();
	protected:
		static cComLibs* pComLibs;

		cComLibs();
		~cComLibs();

		static bool Create();
		static void Destroy();
	}; 
}

