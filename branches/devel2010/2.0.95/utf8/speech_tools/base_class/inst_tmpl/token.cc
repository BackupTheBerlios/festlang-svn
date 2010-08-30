 /************************************************************************/
 /*                                                                      */
 /*                Centre for Speech Technology Research                 */
 /*                     University of Edinburgh, UK                      */
 /*                       Copyright (c) 1996,1997                        */
 /*                        All Rights Reserved.                          */
 /*                                                                      */
 /*  Permission is hereby granted, free of charge, to use and distribute */
 /*  this software and its documentation without restriction, including  */
 /*  without limitation the rights to use, copy, modify, merge, publish, */
 /*  distribute, sublicense, and/or sell copies of this work, and to     */
 /*  permit persons to whom this work is furnished to do so, subject to  */
 /*  the following conditions:                                           */
 /*   1. The code must retain the above copyright notice, this list of   */
 /*      conditions and the following disclaimer.                        */
 /*   2. Any modifications must be clearly marked as such.               */
 /*   3. Original authors' names are not deleted.                        */
 /*   4. The authors' names are not used to endorse or promote products  */
 /*      derived from this software without specific prior written       */
 /*      permission.                                                     */
 /*                                                                      */
 /*  THE UNIVERSITY OF EDINBURGH AND THE CONTRIBUTORS TO THIS WORK       */
 /*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING     */
 /*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT  */
 /*  SHALL THE UNIVERSITY OF EDINBURGH NOR THE CONTRIBUTORS BE LIABLE    */
 /*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   */
 /*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN  */
 /*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,         */
 /*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF      */
 /*  THIS SOFTWARE.                                                      */
 /*                                                                      */
 /*************************************************************************/
 /*                                                                       */
 /*                 Author: Sergio Oller (sergioller@gmail.com)           */
 /*                   Date: Ago 2010                                      */
 /* --------------------------------------------------------------------  */
 /* Instantiate the templates for token functions.                        */
 /*                                                                       */
 /*************************************************************************/


#include "EST_utf8.h"
#include "EST_Token.h"

#if defined(INSTANTIATE_TEMPLATES)

// map class
template class std::_Rb_tree<utf8::uint32_t,std::pair <utf8::uint32_t const, char>, \
	std::_Select1st<std::pair<utf8::uint32_t const, char> >, \
	std::less<utf8::uint32_t>, \
	std::allocator<std::pair<utf8::uint32_t const, char> > >;

//vector class
template class std::vector<unsigned int, std::allocator<unsigned int> >;
template void std::vector<unsigned int, std::allocator<unsigned int> >::_M_range_insert<__gnu_cxx::__normal_iterator<unsigned int*, std::vector<unsigned int, std::allocator<unsigned int> > > >(__gnu_cxx::__normal_iterator<unsigned int*, std::vector<unsigned int, std::allocator<unsigned int> > >, __gnu_cxx::__normal_iterator<unsigned int*, std::vector<unsigned int, std::allocator<unsigned int> > >, __gnu_cxx::__normal_iterator<unsigned int*, std::vector<unsigned int, std::allocator<unsigned int> > >, std::forward_iterator_tag);
template void std::vector<unsigned int, std::allocator<unsigned int> >::_M_assign_aux<__gnu_cxx::__normal_iterator<unsigned int*, std::vector<unsigned int, std::allocator<unsigned int> > > >(__gnu_cxx::__normal_iterator<unsigned int*, std::vector<unsigned int, std::allocator<unsigned int> > >, __gnu_cxx::__normal_iterator<unsigned int*, std::vector<unsigned int, std::allocator<unsigned int> > >, std::forward_iterator_tag);

//deque class
template class std::deque<unsigned int, std::allocator<unsigned int> >;
template void  std::deque<unsigned int, std::allocator<unsigned int> >::_M_insert_aux<std::_Deque_iterator<unsigned int, unsigned int const&, unsigned int const*> >(std::_Deque_iterator<unsigned int, unsigned int&, unsigned int*>, std::_Deque_iterator<unsigned int, unsigned int const&, unsigned int const*>, std::_Deque_iterator<unsigned int, unsigned int const&, unsigned int const*>, unsigned int);
template void  std::deque<unsigned int, std::allocator<unsigned int> >::_M_range_insert_aux<std::_Deque_iterator<unsigned int, unsigned int const&, unsigned int const*> >(std::_Deque_iterator<unsigned int, unsigned int&, unsigned int*>, std::_Deque_iterator<unsigned int, unsigned int const&, unsigned int const*>, std::_Deque_iterator<unsigned int, unsigned int const&, unsigned int const*>, std::forward_iterator_tag);
template void std::_Deque_base<unsigned int, std::allocator<unsigned int> >::_M_create_nodes(unsigned int**, unsigned int**);
template void std::_Deque_base<unsigned int, std::allocator<unsigned int> >::_M_destroy_nodes(unsigned int**, unsigned int**);
template void std::_Deque_base<unsigned int, std::allocator<unsigned int> >::_M_initialize_map(unsigned int);
template      std::_Deque_base<unsigned int, std::allocator<unsigned int> >::~_Deque_base();

template void std::fill<unsigned int>(std::_Deque_iterator<unsigned int, unsigned int&, unsigned int*> const&, std::_Deque_iterator<unsigned int, unsigned int&, unsigned int*> const&, unsigned int const&);


// UTF-8:
template char* utf8::find_invalid<char*>(char*, char*);

Instantiate_Tutf8_next(char*);
template utf8::uint32_t utf8::prior<char*>(char*&, char*);
template char* utf8::append<char*>(utf8::uint32_t, char*);
template utf8::uint32_t utf8::unchecked::next<char*>(char*&);

typedef std::istreambuf_iterator<char, std::char_traits<char> > istreambuf_iterator1;
Instantiate_Tutf8_next(istreambuf_iterator1);

#endif
