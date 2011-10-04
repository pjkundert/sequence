
// 
// sequence-test.C		-- Sequence Arithmetic unit tests
// 
// Copyright (C) 2005 Enbridge Inc.
// 
// This file is part of the Enbridge Sequence Arithmetic Framework
// 
// The Enbridge Sequence Arithmetic Framework unit tests are free software; you
// can redistribute it and/or modify it under the terms of the GNU General
// Public License as published by the Free Software Foundation; either version
// 2, or (at your option) any later version.
// 
// They are distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with UNITS; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
// 

#include <sequence>
#include <cut>

#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <exception>
#include <set>
#include <deque>
#include <map>

#include <stdlib.h>							// for u_int16_t, ...

#if defined( __GNUC__ )
#  define UNUSED		__attribute__(( unused ))
#else
#  define UNUSED
#endif

static char ident[] UNUSED = "@(#) $Id: sequence-test.C,v 1.1 2006-01-16 19:56:44 kundertp Exp $ " __DATE__ "; " __TIME__;

#if defined( TESTSTANDALONE )

// Standalone test suite, if TESTSTANDALONE is defined.  
namespace cut {
    test			root( "Root Sequence Arithmetic unit test suite" );
};

int				main( 
				    int			argc,
				    char const 	      **argv )
{
    // if REQUEST_METHOD environment variable set, assume running as CGI
    bool			cgi	= getenv( "REQUEST_METHOD" );
    cut::runner		       *tests;
    if ( cgi )
	tests				= new cut::htmlrunner( std::cout );
    else
	tests				= new cut::runner( std::cout );

    // run returns true iff all test(s) successful
    if ( argc > 1 ) { 				// Named tests specified?
	for ( int i = 1; i < argc; ++i ) {	// Run each one
	    std::cout << "running test: " << argv[i] << std::endl;
	    if ( ! tests->run( argv[i] )) {
		return 1;
	    }
	}
    } else
	return ! tests->run();			// Run all tests
}

#endif // TESTSTANDALONE


#if defined( TEST )

//
// Unit Tests
// 


// The type Seq are sequence numbers over 16-bit values.  LongSeq defines a total ordering of
// 16-bit sequence numbers over time, mapped onto a 'long long' value.

typedef sequence::number< int16_t, u_int16_t >	
				Seq;
typedef sequence::ordering< int16_t, u_int16_t, long long>
				LongSeq;

namespace cut {

    template < typename S,
	       typename U,
	       typename L >
    inline
    bool	 		equal(
				    const sequence::ordering<S, U, L>
				    		       &lhs,
				    u_int16_t	        rhs )
    {
	return ( ! ( lhs 			< rhs )
		 && ! ( lhs.order( rhs ) 	< (L)lhs ));
    }


    CUT( root,		Sequence_tests, 	"Basic Sequence Arithmetic tests" ) {

	// Basic u_int16_t "wrapping" math.  Ensure that our assumptions about truncation and
	// wrapping are valid.
	u_int16_t		i1	= 12345;
	u_int16_t		i2	= 12346;
	u_int16_t		i3	= 65535;
	u_int16_t		i4	= 0;
	u_int16_t		i5	= 1;
	assert.ISTRUE( i2 == u_int16_t( i1 + 1 ));
	assert.ISTRUE( i2 == u_int16_t( i1 + i5 ));
	assert.ISTRUE( i4 == u_int16_t( i3 + 1 ));
	assert.ISTRUE( i4 == u_int16_t( i3 + i5 ));
	assert.ISTRUE( i4 == u_int16_t( i3 + i5 ));
	assert.ISTRUE( i3 == u_int16_t( i4 - 1 ));
	assert.ISTRUE( i3 == u_int16_t( i4 - i5 ));
	assert.ISTRUE( i5 == u_int16_t( i3 + i5 + i5 ));
	assert.ISTRUE( i5 == u_int16_t( i3 + 2 ));

	assert.ISTRUE( ( i5 -= u_int16_t( 2 )) == i3 );
	assert.ISTRUE( ( i3 += u_int16_t( 1 )) == i4 );
	assert.ISTRUE( ( i2 += i5 ) == i1 );

	{
	    // A few basic tests on Seq
	    Seq			s1( 1 );
	    assert.ISEQUAL( (u_int16_t)s1, u_int16_t( 1 ));
	    assert.ISEQUAL( s1.distance( 32768 ), int16_t(  32767 ));
	    assert.ISEQUAL( s1.distance( 32767 ), int16_t(  32766 ));
	    assert.ISEQUAL( s1.distance(     1 ), int16_t(      0 ));
	    assert.ISEQUAL( s1.distance(     2 ), int16_t(      1 ));
	    assert.ISEQUAL( s1.distance(     0 ), int16_t(     -1 ));
	    assert.ISEQUAL( s1.distance( 65535 ), int16_t(     -2 ));
	    assert.ISEQUAL( s1.distance( 32770 ), int16_t( -32767 ));
	    assert.ISEQUAL( s1.distance( 32769 ), int16_t( -32768 ));
	}

	{
	    // First, lets test the edge cases.  Remember, for any sequence number of N bits, we
	    // have 2^(N-1) considered "less than", but only 2^(N-1)-1 considered "greater than".
	    // Just like 2's complement signed binary arithmetic.  The standard RFC-1982 algorithm
	    // specifies that the comparison against the sequence number that is exactly 2^(N-1)
	    // values away is "undefined".  We define it exactly like 2's complement arithmetic --
	    // it is "less than".  Hence, the computed "distance" will always be negative.
	    Seq			sFFFF( 0xFFFF );
	    Seq			s0000(      0 );
	    Seq			s0001(      1 );
	    Seq			s7FFF( 0x7FFF );
	    Seq			s8000( 0x8000 );
	    Seq			s8001( 0x8001 );

	    assert.ISEQUAL( s0000.distance( 0x7FFF ), int16_t(  32767 ));	// + 2^(N-1)-1  -- largest difference considered "greater than"
	    assert.ISEQUAL( s0000.distance( 0x8000 ), int16_t( -32768 ));	// + 2^(N-1)    -- sequence wrapped! Now "less than"
	    assert.ISEQUAL( s0000.distance( 0x8001 ), int16_t( -32767 ));	// + 2^(N-1)+1  -- still "less than", but getting closer (again)

	    assert.ISEQUAL( s8000.distance( 0xFFFF ), int16_t(  32767 ));
	    assert.ISEQUAL( s8000.distance(      0 ), int16_t( -32768 ));
	    assert.ISEQUAL( s8000.distance(      1 ), int16_t( -32767 ));

	    // As stated in sequence.H, for an N bit sequence number, we define 2^(N-1)-1 values
	    // "above", and 2^(N-1) "below" -- just as in 2's complement binary arithmetic.
	    // Therefore, when two sequence numbers exactly 2^(N-1)-1 apart (eg. 32,767 for 16-bit
	    // sequence numbers), the first values will compare as "<" the second.  As soon as
	    // they become 2^(N-1) apart (eg. 32,768 for 16-bit sequence numbers), the second will
	    // have "wrapped", and suddenly will appear to be "<" than the first.  This is the
	    // nature of sequence number arithmetic -- the problem with the traditional RFC-1982
	    // algorithm is that comparisons of these sequence numbers become "undefined".
	    assert.ISTRUE(  s0000 < s7FFF ); //  32767 
	    assert.ISFALSE( s0000 > s7FFF );
	    assert.ISFALSE( s0000 < s8000 ); // -32768; wrapped!
	    assert.ISTRUE(  s0000 > s8000 );
	    assert.ISFALSE( s0000 < s8001 ); // -32767
	    assert.ISTRUE(  s0000 > s8001 );


	    assert.ISTRUE(  s8000 < sFFFF ); //  32767
	    assert.ISFALSE( s8000 > sFFFF );
	    assert.ISFALSE( s8000 < s0000 ); // -32768; wrapped!
	    assert.ISTRUE(  s8000 > s0000 );
	    assert.ISFALSE( s8000 < s0001 ); // -32767
	    assert.ISTRUE(  s8000 > s0001 );

	    // So, we illustrate the rather odd case: 0x0 > 0x8000, and 0x8000 > 0x0!
	    assert.ISTRUE(  s0000 > s8000 );
	    assert.ISTRUE(  s8000 > s0000 );

	    // However, neither are < each other; so, only using the < operator will automatically
	    // exclude the odd sequence number.  If you are using sequence numbers to (for
	    // example) order things that may have arrived out of order, you can ensure you
	    // exclude an item with sequence number exactly 2^(N-1) distant, by always using the <
	    // operator.  Your algorithm must use other means to ensure a correct total order; for
	    // example, it could ensure that it only generates less than 2^(N-1) items which will
	    // be subject to ordering by the sequence number.
	    assert.ISFALSE( s0000 < s8000 );
	    assert.ISFALSE( s8000 < s0000 );

	    // Another concern in RFC 1982, is that even if you define these edge cases as that
	    // s1<s2, that (s1+1) < (s2+1) will evaluate differently.  Since we are using standard
	    // 2's complement arithmetic, and since we know that a<b === (a+1)<(b+1) for all
	    // values of a and b, we know that our sequence number arithmetic is also consistent.
	    assert.ISTRUE(  Seq( 0x0000 )     < Seq( 0x7FFF ));
	    assert.ISTRUE(  Seq( 0x0000 + 1 ) < Seq( 0x7FFF + 1));
	    assert.ISFALSE( Seq( 0x0000 )     < Seq( 0x8000 ));
	    assert.ISFALSE( Seq( 0x0000 + 1 ) < Seq( 0x8000 + 1));
	}

	{
	    // A few basic tests on LongSeq
	    long long		ord1	= LongSeq( u_int16_t( 0 ), 0x10000 ).order( u_int16_t(     3 ));
	    long long		ord2	= LongSeq( u_int16_t( 0 ), 0x10000 ).order( u_int16_t( 65533 ));
	    assert.ISEQUAL( ord1, (long long)( 0x10000 +     3 ));
	    assert.ISEQUAL( ord2, (long long)( 0x10000 -     3 ));

	    LongSeq		ls3( 65533 );
	    long long		ord3a	= ls3;
	    ls3				= 65534;
	    long long		ord3b	= ls3;
	    assert.ISEQUAL( ord3b, ord3a + 1 );
	    ls3				= 65535;
	    long long		ord3c	= ls3;
	    assert.ISEQUAL( ord3c, ord3b + 1 );
	    ls3				= 0;		// in order, but wrapped; "base" increased by 0x10000 to offset wrapping sequence
	    long long		ord3d	= ls3;
	    assert.ISEQUAL( ord3d, ord3c + 1 );
	    ls3				= 1;
	    long long		ord3e	= ls3;
	    assert.ISEQUAL( ord3e, ord3d + 1 );
	    ls3				= 0;
	    long long		ord3f	= ls3;
	    assert.ISEQUAL( ord3f, ord3e + 0xffff );	// out of order; must force total ordering; "base" increased by 0x10000 for total order

	    LongSeq		ls4(     7 );
	    long long		ord4a	= ls4;
	    long long		ord4b	= ls4.assign( 8 );
	    assert.ISEQUAL( ord4b, ord4a + 1 );
	    long long		ord4c	= ls4.assign( 7 );
	    assert.ISEQUAL( ord4c, ord4b + 0xffff );
	}



	// 
	// Any sequence number +/- any of these numbers will be on the "same side" (always either
	// < or >) the original seqence number.
	// 
	u_int16_t	sameside[]	= {
	    1, 2, 100, 10000, 0x7ffd, 0x7ffe, 0x7fff
	};
	// Try
	u_int16_t	seqs[]	= {
	    0, 1, 2, 100, 1000, 10000, 32765, 32766, 32767, 32768, 32769, 32770, 40000, 50000, 60000, 65533, 65534, 65535
	};	

	for ( size_t s = 0; s < (( sizeof seqs ) / ( sizeof *seqs )); ++s ) {
	    u_int16_t		seq	= seqs[s];
	    LongSeq		ls1( seq, random() % ( seq + 1 ));

	    int  		fails	= assert.stats().failures();

	    // Make sure equality works, both directly in terms of operator==, and implemented in
	    // terms of < and total ordering method LongSeq::order().
	    assert.ISTRUE( ls1 == seq );
	    assert.ISTRUE( (long long) ls1 == ls1.order( seq ));
	    bool		truth	= equal<int16_t, u_int16_t,long long>( ls1, seq );
	    assert.ISTRUE( truth );

	    Seq			s1( seq );
	    assert.ISTRUE( s1 == seq );

	    // Make sure that the </> orientation flips at a distance of exactly 32K, from any base sequence number.
	    assert.ISTRUE( ls1 < u_int16_t( seq + 0x7fff ));		// Room for 32K-1 on > side
	    assert.ISTRUE( ls1 > u_int16_t( seq + 0x8000 ));
	    assert.ISTRUE( ls1 > u_int16_t( seq - 0x8000 ));		// Room for 32K on < side
	    assert.ISTRUE( ls1 < u_int16_t( seq - 0x8001 ));

	    assert.ISTRUE(  s1 < u_int16_t( seq + 0x7fff ));		// Room for 32K-1 on > side
	    assert.ISTRUE(  s1 > u_int16_t( seq + 0x8000 ));
	    assert.ISTRUE(  s1 > u_int16_t( seq - 0x8000 ));		// Room for 32K on < side
	    assert.ISTRUE(  s1 < u_int16_t( seq - 0x8001 ));

	    // Now, ensure that inequality works, too.
	    for ( size_t ss = 0; ss < (( sizeof sameside ) / ( sizeof *sameside )); ++ss ) {
		// These wrap in 16 bits, but should always evaluate correctly (on the same side)
		// when compared to the LongSeq( seq ).

		u_int16_t	gt	= seq + sameside[ss];		// room for 32K-1 on the "greater than" side
		u_int16_t	lt	= seq - sameside[ss] - 1;	// room for one more on the "less than" side


		long long	gtord	= ls1.order( gt );
		assert.ISEQUAL( gtord, (long long)ls1 + sameside[ss] );

		long long 	ltord	= ls1.order( lt );
		assert.ISEQUAL( ltord, (long long)ls1 - sameside[ss] - 1 );


		assert.ISFALSE( ls1 == gt );
		assert.ISFALSE( equal( ls1, gt ));
		assert.ISFALSE( ls1 >  gt );
		assert.ISFALSE( ls1 >= gt );
		assert.ISTRUE(  ls1 <  gt );
		assert.ISTRUE(  ls1 <= gt );

		assert.ISFALSE( ls1 == lt );
		assert.ISFALSE( equal( ls1, lt ));
		assert.ISTRUE(  ls1 >  lt );
		assert.ISTRUE(  ls1 >= lt );
		assert.ISFALSE( ls1 <  lt );
		assert.ISFALSE( ls1 <= lt );

		assert.ISFALSE( s1 == gt );
		assert.ISFALSE( s1 >  gt );
		assert.ISFALSE( s1 >= gt );
		assert.ISTRUE(  s1 <  gt );
		assert.ISTRUE(  s1 <= gt );

		assert.ISFALSE( s1 == lt );
		assert.ISTRUE(  s1 >  lt );
		assert.ISTRUE(  s1 >= lt );
		assert.ISFALSE( s1 <  lt );
		assert.ISFALSE( s1 <= lt );

		if ( fails != assert.stats().failures() )
		    assert.out() << "Testing seq# " 	<< std::setw( 5 ) << seq
				 << " vs. is less: " 	<< std::setw( 5 ) << lt
				 << ", is greater: " 	<< std::setw( 5 ) << gt
				 << "lt over: "	 	<< std::setw( 5 ) 	<< u_int16_t( seq - 0x8001 ) << "(" << ( ls1 < u_int16_t( seq - 0x8001 ) ? "true " : "false" ) << ")"
				 << " lt limit: " 	<< std::setw( 5 ) 	<< u_int16_t( seq - 0x8000 ) << "(" << ( ls1 > u_int16_t( seq - 0x8000 ) ? "true " : "false" ) << ")"
				 << " lt: " 	 	<< std::setw( 5 ) 	<< lt
				 << " seq: "  		<< std::setw( 5 ) 	<< seq
				 << " gt: "  		<< std::setw( 5 ) 	<< gt
				 << " gt limit: "	<< std::setw( 5 ) 	<< u_int16_t( seq + 0x7fff ) << "(" << ( ls1 < u_int16_t( seq + 0x7fff ) ? "true " : "false" ) << ")"
				 << " gt over: "	<< std::setw( 5 ) 	<< u_int16_t( seq + 0x8000 ) << "(" << ( ls1 > u_int16_t( seq + 0x8000 ) ? "true " : "false" ) << ")"
				 << " LongSeq( " 	<< std::setw( 5 ) 	<< seq << " ): " << (long long)ls1
				 << " .order( " 	<< std::setw( 5 ) 	<< u_int16_t( seq - 0x8001 ) << " ) == " << ls1.order( u_int16_t( seq - 0x8001 ))
				 << " .order( " 	<< std::setw( 5 ) 	<< u_int16_t( seq - 0x8000 ) << " ) == " << ls1.order( u_int16_t( seq - 0x8000 ))
				 << " .order( " 	<< std::setw( 5 ) 	<< lt << " ) == " << ls1.order( lt )
				 << " .order( " 	<< std::setw( 5 ) 	<< gt << " ) == " << ls1.order( gt )
				 << " .order( " 	<< std::setw( 5 ) 	<< u_int16_t( seq + 0x7fff ) << " ) == " << ls1.order( u_int16_t( seq - 0x7fff ))
				 << " .order( " 	<< std::setw( 5 ) 	<< u_int16_t( seq + 0x8000 ) << " ) == " << ls1.order( u_int16_t( seq - 0x8000 ))
				 << " Seq( " 		<< std::setw( 5 ) 	<< seq << " ): " << (u_int16_t)s1
				 << " .dist.( " 	<< std::setw( 5 ) 	<< u_int16_t( seq - 0x8001 ) << " ) == " << s1.distance( u_int16_t( seq - 0x8001 ))
				 << " .dist.( " 	<< std::setw( 5 ) 	<< u_int16_t( seq - 0x8000 ) << " ) == " << s1.distance( u_int16_t( seq - 0x8000 ))
				 << " .dist.( " 	<< std::setw( 5 ) 	<< lt << " ) == " << s1.distance( lt )
				 << " .dist.( " 	<< std::setw( 5 ) 	<< gt << " ) == " << s1.distance( gt )
				 << " .dist.( " 	<< std::setw( 5 ) 	<< u_int16_t( seq + 0x7fff ) << " ) == " << s1.distance( u_int16_t( seq - 0x7fff ))
				 << " .dist.( " 	<< std::setw( 5 ) 	<< u_int16_t( seq + 0x8000 ) << " ) == " << s1.distance( u_int16_t( seq - 0x8000 ))
				 << std::endl;
		assert.ISEQUAL( fails, assert.stats().failures() );
		fails			= assert.stats().failures();
	    }

	    fails			= assert.stats().failures();
	    LongSeq		ls0( ls1 );

	    // Try assignment, and pre/post increment
	    long long		o1	= ls1;
	    assert.ISTRUE( u_int16_t( ls1 ) == seq );

	    long long		o2	= ++ls1;
	    assert.ISTRUE( u_int16_t( ls1 ) == u_int16_t( seq + 1 ));

	    long long		o3	= ls1;
	    long long		o4	= ls1++;
	    assert.ISTRUE( u_int16_t( ls1 ) == u_int16_t( seq + 2 ));

	    long long		o5	= ls1;
	    assert.ISTRUE( o1 <  o2 );
	    assert.ISTRUE( o2 == o3 );
	    assert.ISTRUE( o3 == o4 );
	    assert.ISTRUE( o4 <  o5 );

	    if ( fails != assert.stats().failures() ) {
		assert.out() << " ls1 (before): " << ls1 
			     << ", u_int16_t( ls1 ): " << std::setw( 5 ) << u_int16_t( ls1 )
			     << ", u_int16_t( ls1 ) + u_int16_t( 10 ): " << std::setw( 5 ) << ( u_int16_t( ls1 ) + u_int16_t( 10 ) )
			     << std::endl;
	    }
											  
	    ls1				= u_int16_t( ls1 ) + u_int16_t( 10 );// OK; sequence number is in simple order
	    long long		o6	= ls1;

	    assert.ISTRUE( o5 <  o6 );
	    
	    ls1				= seq;				// Will not go "back" in total order; will advance to future "wrapped" version of the sequence number
	    long long		o7	= ls1;
	    assert.ISTRUE( u_int16_t( ls1 ) == seq );
	    assert.ISTRUE( o6 <  o7 );

	    assert.ISTRUE( int( ls1 ) == int( seq ));			// Invalid; compiler will reject

	    if ( fails != assert.stats().failures() ) {
		assert.out() << "ls0: " << ls0 << std::endl;
		assert.out() << "ls1: " << ls1 << std::endl;
	    }
	    assert.ISEQUAL( fails, assert.stats().failures() );
	}
    }
} // namespace cut

#endif // TEST
