/* TestFilterFactory.H
 */

#ifndef _TEST_FILTER_FACTORY_H_
#define _TEST_FILTER_FACTORY_H_

#include "FilterFactory.H"
#include "TestFilter.H"

namespace nle
{

class TestFilterFactory : public FilterFactory
{
	public:
		const char* name() { return "TestFilter"; }
		FilterBase* get( Clip* clip ) { return new TestFilter( clip ); }
		~TestFilterFactory() {}
};

} /* namespace nle */

#endif /* _TEST_FILTER_FACTORY_H_ */

