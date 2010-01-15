/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

//	Gathers unpredictable system data to be used for generating
//	random bits

#include <MacTypes.h>

class CRandomizer
{
public:
	CRandomizer (void);
	void PeriodicAction (void);
	
private:

	// Private calls

	void		AddTimeSinceMachineStartup (void);
	void		AddAbsoluteSystemStartupTime (void);
	void		AddAppRunningTime (void);
	void		AddStartupVolumeInfo (void);
	void		AddFiller (void);

	void		AddCurrentMouse (void);
	void		AddNow (double millisecondUncertainty);
	void		AddBytes (void *data, long size, double entropy);
	
	void		GetTimeBaseResolution (void);
	unsigned long	SysTimer (void);

	// System Info	
	bool		mSupportsLargeVolumes;
	bool		mIsPowerPC;
	bool		mIs601;
	
	// Time info
	double		mTimebaseTicksPerMillisec;
	unsigned long	mLastPeriodicTicks;
	
	// Mouse info
	long		mSamplePeriod;
	Point		mLastMouse;
	long		mMouseStill;
};
