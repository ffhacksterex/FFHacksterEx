#pragma once

namespace Io
{

	class SetResourceHandleScope
	{
	public:
		SetResourceHandleScope(HMODULE newmodule);
		~SetResourceHandleScope();

		void Set(HMODULE newmodule);

		operator HMODULE() const;

	private:
		HMODULE m_module;
		HMODULE m_origmodule;

		void Restore();
	};

}
