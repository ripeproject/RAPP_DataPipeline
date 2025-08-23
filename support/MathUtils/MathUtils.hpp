#pragma once

#include <string>
#include <limits>


namespace nMathUtils
{
	// Wrap the angle to the range of -pi to pi
	double wrap_neg_pi_to_pi(double angle_rad);

	// Wrap the angle to the range of 0 to two pi
	double wrap_0_to_two_pi(double angle_rad);

	// Wrap the angle to the range of 0 to 360
	double wrap_0_to_360(double angle_deg);

	// Wrap the angle to the range of -180 to 180
	double wrap_neg_180_to_180(double angle_deg);

	template<typename T>
	T bound(T value, T lower_bound, T upper_bound)
	{
		if (value > upper_bound) return upper_bound;
		if (value < lower_bound) return lower_bound;

		return value;
	}

	template<typename T0, typename T1>
	T0 bound(T1 value)
	{
		if (value > std::numeric_limits<T0>::max()) return std::numeric_limits<T0>::max();
		if (value < std::numeric_limits<T0>::lowest()) return std::numeric_limits<T0>::lowest();

		return static_cast<T0>(value);
	}

	template<typename T>
	T threshold(T min, T max, double level_pct)
	{
		if (level_pct > 100.0) return max;
		if (level_pct < 0.0) return min;

		return static_cast<T>((max - min) * (level_pct / 100.0)) + min;
	}



    //////////////////////////////////////////////////////////////////////////////
    /**
    //  \name  edge_detect template
    */
    //////////////////////////////////////////////////////////////////////////////
    //@{
    /**
    //  \brief Performs edge detection
    //
    //  \par  Template Function
    //        Implemented as a template to work with any variable type.  The template
    //        overloads the basic operators so that the template behaves like the
    //        underlying variable type.  The template uses a traits template to
    //        control the threshold at which a rising or falling edge is detected.
    //
    //  \verbatim
    //        The following is usage of the edge_detect template:
    //
    //        edge_detect<bool>   b;
    //
    //        b = false;
    //        if ( !b )
    //           cout << b.IsFalling() << endl;
    //
    //
    //  \endverbatim
    //
    //  \param typename  - The type of the value.
    //  \param traits    - A traits class to determine threshold values for the rising and
    //                     falling edges.
    //
    */

    /**
    *  \template edge_traits template
    *
    *  \brief    Implements default behavior for edge detection
    */
    template<typename T>
    struct edge_traits
    {
        static bool lt(const T& current, const T& previous) { return (current < previous); };
        static bool gt(const T& current, const T& previous) { return (current > previous); };
        static bool eq(const T& current, const T& previous) { return (current == previous); };
    };

    /**
    *  \template edge_traits<bool> template
    *
    *  \brief    Implements edge detection for boolean values
    */
    template<>
    struct edge_traits<bool>
    {
        static bool lt(const bool& current, const bool& previous) { return (!current && previous); };
        static bool gt(const bool& current, const bool& previous) { return (current && !previous); };
        static bool eq(const bool& current, const bool& previous) { return (current == previous); };
    };

    /**
    *  \template edge_detect<typename TYPE, class TRAITS> template
    *
    *  \brief    Implements edge detection for a variable of TYPE, using the traits
    *            from TRAITS.
    */
    template<typename T, class Tr = edge_traits<T> >
    class edge_detect
    {
    public:
        typedef edge_detect<T, Tr>   _Myt;
        typedef T                    value_type;
        typedef Tr                   traits_type;

        edge_detect() : current_value(), previous_value() {}

        edge_detect(const T& x) : current_value(x), previous_value(x) {}

        edge_detect(const _Myt& X) : current_value(X.current_value), previous_value(X.previous_value) {}

        _Myt& operator=(const _Myt& rhs)
        {
            if (&rhs == this) return (*this);
            current_value = rhs.current_value;
            previous_value = rhs.previous_value;
            return (*this);
        }

        _Myt& operator=(const T& value)
        {
            previous_value = current_value;
            current_value = value;
            return (*this);
        }

        _Myt& operator|=(const T& value)
        {
            previous_value = current_value;
            current_value |= value;
            return (*this);
        }

        _Myt& operator&=(const T& value)
        {
            previous_value = current_value;
            current_value &= value;
            return (*this);
        }

        //    operator T()        { return current_value; }
        operator T() const { return current_value; }

        bool IsRising() const { return Tr::gt(current_value, previous_value); }
        bool IsFalling() const { return Tr::lt(current_value, previous_value); }
        bool HasChanged() const { return !Tr::eq(current_value, previous_value); }

        // The reset method will keep the current value, but clear the 
        // IsRising, IsFalling and HasChanged methods to return false.
        void reset() { previous_value = current_value; }

        // The reset method will set the current and previous values.
        // Thus, the IsRising, IsFalling and HasChanged methods to return false.
        void reset(T value) 
        { 
            current_value = value;
            previous_value = current_value; 
        }

        // The data method will bypass the edge-detection capabilities of this class.
        // Use it with caution!
        T& data() { return current_value; }

        // The previous method will bypass the edge-detection capabilities of this class.
        // Use it with caution!
        T& previous() { return previous_value; }
        void previous(const T& e) { previous_value = e; }

        /*
             std::istream& operator>>(std::istream& is)
             {
                previous_value = current_value;
                is >> current_value;
                return is;
             }

             std::ostream& operator<<(std::ostream& os)
             {
                os << current_value;
                return os;
             }
        */

    protected:
        T current_value;
        T previous_value;
    };


} // End of Namespace nMathUtils