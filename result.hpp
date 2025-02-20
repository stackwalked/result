#pragma once

namespace kita {
    enum err_t {
        RES_NONE = 0, // theoretically unused due to std::expected being used instead
        RES_ERR_INITHOOKS,
        RES_ERR_SYSTEMIO,
        RES_ERRUNKNOWN,
    };

    constexpr auto to_string( err_t error ) -> std::string {
        switch ( error ) {
        case RES_NONE:
            return "err_t: none";
        case RES_ERR_SYSTEMIO:
            return "err_t: system I/O error";
        case RES_ERR_INITHOOKS:
            return "err_t: error initializing hooks";
        default:
            return "err_t: (unsupported error type)";
        }
    }

    constexpr auto is_critical( err_t error ) -> bool {
        switch ( error ) {
        case RES_ERR_INITHOOKS:
            return true;
        default:
            return false;
        }
    }
}

template < >
struct std::formatter< kita::err_t > : formatter< std::string > {
    using std::formatter< std::string >::parse;

    auto format( kita::err_t error, format_context &ctx ) const {
        return std::format_to( ctx.out( ), "{}", to_string( error ) );
    }
};

namespace kita {

    template < typename T >
    using res_t = std::expected< T, err_t >;

    using fail = std::unexpected< err_t >;

    enum class result_handler_t {
        except,
        print, // this is specialized in ext/debug/logger.hpp due to depending on it
        mbox
    };

    template < result_handler_t handler_type >
    res_t< void > result_handler( err_t error ) {
        // use throwing handler by default i guess
        return result_handler< result_handler_t::except >( error );
    }

    template < >
    inline auto result_handler< result_handler_t::except >( err_t error ) -> res_t< void > {
        throw std::runtime_error( std::format( "result_handler_t::THROW -> {}", error ) );
    }

    template < >
    inline auto result_handler< result_handler_t::mbox >( err_t error ) -> res_t< void > {
        MessageBoxA( nullptr, std::format( "{}", error ).c_str( ), k_project_name,
                     MB_ICONERROR | MB_OK );

        // proceed like nothing happened
        return res_t< void >( );
    }
} // namespace kita

