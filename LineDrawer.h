#ifndef LINEDRAWER_H
#define LINEDRAWER_H

#include <iostream>
#include <cmath>

class LineDrawer
{
public:
	LineDrawer(int sx, int sy, int ex, int ey):
		m_CurX(sx), m_CurY(sy),
		m_EndX(ex), m_EndY(ey)
	{
		m_StepX = (ex > sx) ? 1 : -1;
		m_StepY = (ey > sy) ? 1 : -1;
		m_dx = abs(ex - sx);
		m_dy = abs(ey - sy);
		m_Eps = 0;
	}

	void nextStep()
	{
		if (m_dx > m_dy)
		{
			m_Eps += m_dy;
			if (m_Eps * 2 >= m_dx)
			{
				m_CurY += m_StepY;
				m_Eps -= m_dx;
			}
			m_CurX += m_StepX;
		}
		else
		{
			m_Eps += m_dx;
			if (m_Eps * 2 >= m_dy)
			{
				m_CurX += m_StepX;
				m_Eps -= m_dy;
			}
			m_CurY += m_StepY;
		}
	}

	int x() const { return m_CurX; }
	int y() const { return m_CurY; }
	bool finished() const { return m_CurX == m_EndX && m_CurY == m_EndY; }

private:
	int m_EndX, m_EndY;
	int m_CurX, m_CurY;
	int m_StepX, m_StepY;
	int m_dx, m_dy;
	int m_Eps;
};

#endif
