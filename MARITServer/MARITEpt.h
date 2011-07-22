#pragma once

class MARITEpt {
public:
	MARITEpt() : m_pMessage("") {}
	virtual ~MARITEpt() {}
	MARITEpt(const char* pMessage) : m_pMessage(pMessage) {}
	const char* what() { return m_pMessage; }

private:
	const char* m_pMessage;
};
