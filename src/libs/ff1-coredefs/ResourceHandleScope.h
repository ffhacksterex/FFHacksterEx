#pragma once

namespace Io {

	//N.B. - this class' dtor always calls AfxSetResourceHandle to restore the current resource handle.
	class ResourceHandleScope
	{
	public:
		ResourceHandleScope(HMODULE newmodule);
		~ResourceHandleScope();

		void Set(HMODULE newmodule);
		operator HMODULE() const;

	private:
		HMODULE m_module;
		HMODULE m_origmodule;

		void Restore();
	};

}
