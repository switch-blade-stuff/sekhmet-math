
list(APPEND SEK_MATH_INTERFACE_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/define.hpp
        ${CMAKE_CURRENT_LIST_DIR}/fwd.hpp
        ${CMAKE_CURRENT_LIST_DIR}/shuffle.hpp
        ${CMAKE_CURRENT_LIST_DIR}/type_vec.hpp
        ${CMAKE_CURRENT_LIST_DIR}/type_mat.hpp
        ${CMAKE_CURRENT_LIST_DIR}/blend.hpp
        ${CMAKE_CURRENT_LIST_DIR}/math_fwd.hpp
        ${CMAKE_CURRENT_LIST_DIR}/mbase.hpp
        ${CMAKE_CURRENT_LIST_DIR}/power.hpp
        ${CMAKE_CURRENT_LIST_DIR}/expon.hpp
        ${CMAKE_CURRENT_LIST_DIR}/trig.hpp
        ${CMAKE_CURRENT_LIST_DIR}/geom.hpp
        ${CMAKE_CURRENT_LIST_DIR}/hypbl.hpp
        ${CMAKE_CURRENT_LIST_DIR}/errfn.hpp
        ${CMAKE_CURRENT_LIST_DIR}/neari.hpp
        ${CMAKE_CURRENT_LIST_DIR}/fmanip.hpp
        ${CMAKE_CURRENT_LIST_DIR}/fclass.hpp
        ${CMAKE_CURRENT_LIST_DIR}/fcmp_vec.hpp
        ${CMAKE_CURRENT_LIST_DIR}/fcmp_mat.hpp
        ${CMAKE_CURRENT_LIST_DIR}/inverse.hpp
        ${CMAKE_CURRENT_LIST_DIR}/trans.hpp
        ${CMAKE_CURRENT_LIST_DIR}/xoroshiro.hpp)

list(APPEND SEK_MATH_PUBLIC_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/sysrandom.hpp)
list(APPEND SEK_MATH_PRIVATE_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/sysrandom.cpp)