/* LiftGammaGainWrapperWidget.cxx
 *
 *  Copyright (C) 2009 Richard Spindler <richard.spindler AT gmail.com>
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

#include "LiftGammaGainWrapperWidget.H"
#include "LiftGammaGainWidget.H"
#include "LiftGammaGainFilter.H"
#include "VideoViewGL.H"

namespace nle
{

LiftGammaGainWrapperWidget::LiftGammaGainWrapperWidget( LiftGammaGainFilter* filter )
	: IEffectWidget( 0, 0, 585, 165 )
{
	m_filter = filter;
	m_widget = new LiftGammaGainWidget( 0, 0, 585, 165 );
	m_widget->lift->rgb( m_filter->lift()[0], m_filter->lift()[1], m_filter->lift()[2] );
	m_widget->gamma->rgb( m_filter->gamma()[0], m_filter->gamma()[1], m_filter->gamma()[2] );
	m_widget->gain->rgb( m_filter->gain()[0], m_filter->gain()[1], m_filter->gain()[2] );
	m_widget->lift_slider->value( m_filter->lift()[3] );
	m_widget->gamma_slider->value( m_filter->gamma()[3] );
	m_widget->gain_slider->value( m_filter->gain()[3] );
	m_widget->m_dialog = this;
}
LiftGammaGainWrapperWidget::~LiftGammaGainWrapperWidget()
{
}
void LiftGammaGainWrapperWidget::read_values()
{
	m_filter->lift( m_widget->lift->r(), m_widget->lift->g(), m_widget->lift->b(), m_widget->lift_slider->value() );
	m_filter->gamma( m_widget->gamma->r(), m_widget->gamma->g(), m_widget->gamma->b(), m_widget->gamma_slider->value() );
	m_filter->gain( m_widget->gain->r(), m_widget->gain->g(), m_widget->gain->b(), m_widget->gain_slider->value() );
	m_filter->calculate_values();

	g_videoView->redraw();
}
	

} /* namespace nle */
