/*  JobManager.cxx
 *
 *  Copyright (C) 2008 Richard Spindler <richard.spindler AT gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _JOB_MANAGER_H_
#define _JOB_MANAGER_H_

namespace nle
{

class Job;

class JobDoneListener
{
	public:
		virtual void jobDone( Job* job_description ) = 0;
};

class Job
{
	public:
		virtual ~Job() {}
		virtual bool process( double& percentage ) = 0;
		virtual void cancel() = 0;
		virtual bool done() = 0;
		virtual const char* name() { return "Generic Job"; }
		virtual const char* description() { return "Generic Job Description"; }
		void callJobDone() { if (!m_listener) { return; } m_listener->jobDone( this ); }
	protected:
		Job( JobDoneListener* listener = 0 ) { m_listener = listener; }
	private:
		JobDoneListener* m_listener;
		IProgressListener
};

class IJobManager
{
	public:
		virtual ~IJobManager();
		virtual void submitJob( Job* job_description ) = 0;
	protected:
		IJobManager() {}
	private:
};

class JobManagerIdle : public IJobManager, public IdleHandler
{
	public:
		JobManagerIdle();
		~JobManagerIdle();
		bool process();
		void submitJob( Job* job_description );
	private:
};

} /* namespace nle */

#endif /* _JOB_MANAGER_H_ */
