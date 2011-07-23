# coding=utf-8
#
# Copyright © 2011 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.

# This source file defines a set of test vectors that can be used to
# test GLSL's built-in functions.  It is intended to be used by
# Python code that generates Piglit tests.
#
# The key export is the dictionary test_suite.  It contains an entry
# for each possible overload of every pure built-in function.  By
# iterating through this dictionary you can find a set of test vectors
# for testing nearly every built-in GLSL function.  Notable exceptions
# include the fragment shader functions dFdx(), dFdy(), and fwidth(),
# the texture lookup functions, and the ftransform() function, since
# they are not pure, so they can't be tested using simple test
# vectors.

import collections
import itertools
import numpy as np



# Floating point types used by Python and numpy
FLOATING_TYPES = (float, np.float64, np.float32)



class GlslBuiltinType(object):
    """Class representing a GLSL built-in type."""
    def __init__(self, name, base_type, num_cols, num_rows,
		 version_introduced):
	self.__name = name
	if base_type is not None:
	    self.__base_type = base_type
	else:
	    self.__base_type = self
	self.__num_cols = num_cols
	self.__num_rows = num_rows
	self.__version_introduced = version_introduced

    @property
    def name(self):
	"""The name of the type, as a string."""
	return self.__name

    @property
    def base_type(self):
	"""For vectors and matrices, the type of data stored in each
	element.  For scalars, equal to self.
	"""
	return self.__base_type

    @property
    def num_cols(self):
	"""For matrices, the number of columns.  For vectors and
	scalars, 1.
	"""
	return self.__num_cols

    @property
    def num_rows(self):
	"""For vectors and matrices, the number of rows.  For scalars,
	1.
	"""
	return self.__num_rows

    @property
    def is_scalar(self):
	return self.__num_cols == 1 and self.__num_rows == 1

    @property
    def is_vector(self):
	return self.__num_cols == 1 and self.__num_rows != 1

    @property
    def is_matrix(self):
	return self.__num_cols != 1

    @property
    def version_introduced(self):
	"""The earliest version of GLSL that this type appears in (as
	a string, e.g. '1.10').
	"""
	return self.__version_introduced

    def __str__(self):
	return self.__name

    def __repr__(self):
	return 'glsl_{0}'.format(self.__name)



# Concrete declarations of GlslBuiltinType
glsl_bool   = GlslBuiltinType('bool',   None,       1, 1, '1.10')
glsl_int    = GlslBuiltinType('int',    None,       1, 1, '1.10')
glsl_float  = GlslBuiltinType('float',  None,       1, 1, '1.10')
glsl_vec2   = GlslBuiltinType('vec2',   glsl_float, 1, 2, '1.10')
glsl_vec3   = GlslBuiltinType('vec3',   glsl_float, 1, 3, '1.10')
glsl_vec4   = GlslBuiltinType('vec4',   glsl_float, 1, 4, '1.10')
glsl_bvec2  = GlslBuiltinType('bvec2',  glsl_bool,  1, 2, '1.10')
glsl_bvec3  = GlslBuiltinType('bvec3',  glsl_bool,  1, 3, '1.10')
glsl_bvec4  = GlslBuiltinType('bvec4',  glsl_bool,  1, 4, '1.10')
glsl_ivec2  = GlslBuiltinType('ivec2',  glsl_int,   1, 2, '1.10')
glsl_ivec3  = GlslBuiltinType('ivec3',  glsl_int,   1, 3, '1.10')
glsl_ivec4  = GlslBuiltinType('ivec4',  glsl_int,   1, 4, '1.10')
glsl_mat2   = GlslBuiltinType('mat2',   glsl_float, 2, 2, '1.10')
glsl_mat3   = GlslBuiltinType('mat3',   glsl_float, 3, 3, '1.10')
glsl_mat4   = GlslBuiltinType('mat4',   glsl_float, 4, 4, '1.10')
glsl_mat2x2 = glsl_mat2
glsl_mat3x2 = GlslBuiltinType('mat3x2', glsl_float, 3, 2, '1.20')
glsl_mat4x2 = GlslBuiltinType('mat4x2', glsl_float, 4, 2, '1.20')
glsl_mat2x3 = GlslBuiltinType('mat2x3', glsl_float, 2, 3, '1.20')
glsl_mat3x3 = glsl_mat3
glsl_mat4x3 = GlslBuiltinType('mat4x3', glsl_float, 4, 3, '1.20')
glsl_mat2x4 = GlslBuiltinType('mat2x4', glsl_float, 2, 4, '1.20')
glsl_mat3x4 = GlslBuiltinType('mat3x4', glsl_float, 3, 4, '1.20')
glsl_mat4x4 = glsl_mat4



# Named tuple representing the signature of a single overload of a
# built-in GLSL function:
# - name is the function name.
# - version_introduced earliest version of GLSL the test applies to
#   (as a string, e.g. '1.10').
# - rettype is the return type of the function (as a GlslBuiltinType).
# - argtypes is a tuple containing the types of each function
#   parameter (as GlslBuiltinTypes).
#
# For example, the function
#
#   vec3 step(float edge, vec3 x)
#
# has a signature of
#
# Signature(name='step', version_introduced='1.10', rettype='vec3',
#           argtypes=('float', 'vec3'))
Signature = collections.namedtuple(
    'Signature', ('name', 'version_introduced', 'rettype', 'argtypes'))



# Named tuple representing a single piece of test data for testing a
# built-in GLSL function:
# - arguments is a tuple containing the arguments to apply to the
#   function.  Each argument is of a type native to numpy (e.g.
#   numpy.float32 or numpy.ndarray)
# - result is the value the function is expected to return.  It is
#   also of a type native to numpy.
# - tolerance is a float32 representing how much deviation from the
#   result we expect, considering the floating point precision
#   requirements of GLSL and OpenGL.  The value may be zero for test
#   vectors involving booleans and integers.  If result is a vector or
#   matrix, tolerance should be interpreted as the maximum permissible
#   RMS error (as would be computed by the distance() function).
TestVector = collections.namedtuple(
    'TestVector', ('arguments', 'result', 'tolerance'))



def glsl_type_of(value):
    """Return the GLSL type corresponding to the given native numpy
    value, as a GlslBuiltinType.
    """
    if isinstance(value, FLOATING_TYPES):
	return glsl_float
    elif isinstance(value, (bool, np.bool_)):
	return glsl_bool
    elif isinstance(value, (int, long)):
	return glsl_int
    else:
	assert isinstance(value, np.ndarray)
	if len(value.shape) == 1:
	    # Vector
	    vector_length = value.shape[0]
	    assert 2 <= vector_length <= 4
	    if value.dtype in FLOATING_TYPES:
		return (glsl_vec2, glsl_vec3, glsl_vec4)[vector_length - 2]
	    elif value.dtype == bool:
		return (glsl_bvec2, glsl_bvec3, glsl_bvec4)[vector_length - 2]
	    elif value.dtype == int:
		return (glsl_ivec2, glsl_ivec3, glsl_ivec4)[vector_length - 2]
	    else:
		raise Exception(
		    'Unexpected vector base type {0}'.format(value.dtype))
	else:
	    # Matrix
	    assert value.dtype in FLOATING_TYPES
	    assert len(value.shape) == 2
	    matrix_rows = value.shape[0]
	    assert 2 <= matrix_rows <= 4
	    matrix_columns = value.shape[1]
	    assert 2 <= matrix_columns <= 4
	    matrix_types = ((glsl_mat2x2, glsl_mat2x3, glsl_mat2x4),
			    (glsl_mat3x2, glsl_mat3x3, glsl_mat3x4),
			    (glsl_mat4x2, glsl_mat4x3, glsl_mat4x4))
	    return matrix_types[matrix_columns - 2][matrix_rows - 2]



def column_major_values(value):
    """Given a native numpy value, return a list of the scalar values
    comprising it, in column-major order."""
    if isinstance(value, np.ndarray):
	return list(np.reshape(value, -1, 'F'))
    else:
	return [value]



def glsl_constant(value):
    """Given a native numpy value, return GLSL code that constructs
    it."""
    column_major = np.reshape(np.array(value), -1, 'F')
    if column_major.dtype == bool:
	values = ['true' if x else 'false' for x in column_major]
    else:
	values = [repr(x) for x in column_major]
    if len(column_major) == 1:
	return values[0]
    else:
	return '{0}({1})'.format(glsl_type_of(value), ', '.join(values))



def round_to_32_bits(value):
    """If value is a floating point type, round it down to 32 bits.
    Otherwise return it unchanged.
    """
    if isinstance(value, float):
	return np.float32(value)
    elif isinstance(value, np.ndarray) and value.dtype == np.float64:
	return np.array(value, dtype=np.float32)
    else:
	return value



def extend_to_64_bits(value):
    """If value is a floating point type, extend it to 64 bits.
    Otherwise return it unchanged.
    """
    if isinstance(value, np.float32):
	return np.float64(value)
    elif isinstance(value, np.ndarray) and value.dtype == np.float32:
	return np.array(value, dtype=np.float64)
    else:
	return value



# Dictionary containing the test vectors.  Each entry in the
# dictionary represents a single overload of a single built-in
# function.  Its key is a Signature tuple, and its value is a list of
# TestVector tuples.
#
# Note: the dictionary is initialized to {} here, but it is filled
# with test vectors by code later in this file.
test_suite = {}



# Implementation
# ==============
#
# The functions below shouldn't be necessary to call from outside this
# file.  They exist solely to populate test_suite with test vectors.

# Functions that simulate GLSL built-in functions (in the cases where
# the GLSL built-in functions have no python or numpy equivalent, or
# in cases where there is a behavioral difference).  These functions
# return None if the behavior of the GLSL built-in is undefined for
# the given set of inputs.
def _arctan2(y, x):
    if x == y == 0.0:
	return None
    return np.arctan2(y, x)
def _pow(x, y):
    if x < 0.0:
	return None
    if x == 0.0 and y <= 0.0:
	return None
    return np.power(x, y)
def _clamp(x, minVal, maxVal):
    if minVal > maxVal:
	return None
    return min(max(x, minVal), maxVal)
def _smoothstep(edge0, edge1, x):
    if edge0 >= edge1:
	return None
    t = _clamp((x-edge0)/(edge1-edge0),0.0,1.0)
    return t*t*(3.0-2.0*t)
def _normalize(x):
    return x/np.linalg.norm(x)
def _faceforward(N, I, Nref):
    if np.dot(Nref, I) < 0.0:
	return N
    else:
	return -N
def _reflect(I, N):
    return I-2*np.dot(N,I)*N
def _refract(I, N, eta):
    k = 1.0-eta*eta*(1.0-np.dot(N,I)*np.dot(N,I))
    if k < 0.0:
	return I*0.0
    else:
	return eta*I-(eta*np.dot(N,I)+np.sqrt(k))*N



def _argument_types_match(arguments, argument_indices_to_match):
    """Return True if all of the arguments indexed by
    argument_indices_to_match have the same GLSL type.
    """
    types = [glsl_type_of(arguments[i]) for i in argument_indices_to_match]
    return all(x == types[0] for x in types)



def _strict_tolerance(arguments, result):
    """Compute tolerance using a strict interpretation of the GLSL and
    OpenGL standards.

    From the GLSL 1.20 spec (4.1.4 "Floats"):

      "As an input value to one of the processing units, a
      floating-point variable is expected to match the IEEE single
      precision floating-point definition for precision and dynamic
      range.  It is not required that the precision of internal
      processing match the IEEE floating-point specification for
      floating-point operations, but the guidelines for precision
      established by the OpenGL 1.4 specification must be met."

    From the OpenGL 1.4 spec (2.1.1 "Floating-Point Computation"):

      "We require simply that numbers' floating-point parts contain
      enough bits ... so that individual results of floating-point
      operations are accurate to about 1 part in 10^5."

    A harsh interpretation of the above is that (a) no precision is
    lost in moving numbers into or out of the GPU, and (b) any
    built-in function constitutes a single operation, so therefore the
    error in applying any built-in function should be off by no more
    than 1e-5 times its theoretically correct value.

    This is not the only possible interpretation, however.  Certain
    built-in functions, such as the cross product, are computed by a
    formula consisting of many elementary multiplications and
    additions, in which a large amount of cancellation sometimes
    occurs.  It's possible that these rules are meant to apply to
    those elementary multiplications and additions, and not the full
    built-in function. Other built-in functions, such as the trig
    functions, are typically implemented by a series approximation, in
    which 1 part in 10^5 accuracy seems like overkill.  See below for
    the tolerance computation we use on these other functions.
    """
    return 1e-5 * np.linalg.norm(result)



def _trig_tolerance(arguments, result):
    """Compute a more lenient tolerance bound for trig functions.

    The GLSL and OpenGL specs don't provide any guidance as to the
    required accuracy of trig functions (other than the "1 part in
    10^5" general accuracy requirement, which seems like overkill for
    trig functions.

    So the tolerance here is rather arbitrarily chosen to be either 1
    part in 10^3 or 10^-4, whichever is larger.
    """
    return max(1e-4, 1e-3 * np.linalg.norm(result))



def _cross_product_tolerance(arguments, result):
    """Compute a more lenient tolerance bound for cross product.

    Since the computation of a cross product may involve a large
    amount of cancellation, an error tolerance of 1 part in 10^5
    (referred to the magnitude of the result vector) is overly tight.

    So instead we allow the error to be 1 part in 10^5 referred to the
    product of the magnitudes of the arguments.
    """
    assert len(arguments) == 2
    return 1e-5 * np.linalg.norm(arguments[0]) * np.linalg.norm(arguments[1])



def _simulate_function(test_inputs, python_equivalent, tolerance_function):
    """Construct test vectors by simulating a GLSL function on a list
    of possible inputs, and return a list of test vectors.

    test_inputs is a list of possible input sequences, each of which
    represents a set of arguments that should be applied to the
    function.

    python_equivalent is the function to simulate--it should return
    None if the GLSL function returns undefined results for the given
    set of inputs, otherwise it should return the expected result.
    Input sequences for which python_equivalent returns None are
    ignored.

    The function is simulated using 64 bit floats for maximum possible
    accuracy, but the output is rounded to 32 bits since that is the
    data type that we expect to get back form OpenGL.

    tolerance_function is the function to call to compute the
    tolerance.  It should take the set of arguments and the expected
    result as its parameters.  It is only used for functions that
    return floating point values.
    """
    test_vectors = []
    for inputs in test_inputs:
	expected_output = round_to_32_bits(
	    python_equivalent(*[extend_to_64_bits(x) for x in inputs]))
	if expected_output is not None:
	    if glsl_type_of(expected_output).base_type != glsl_float:
		tolerance = np.float32(0.0)
	    else:
		tolerance = np.float32(
		    tolerance_function(inputs, expected_output))
	    test_vectors.append(TestVector(inputs, expected_output, tolerance))
    return test_vectors



def _vectorize_test_vectors(test_vectors, scalar_arg_indices, vector_length):
    """Build a new set of test vectors by combining elements of
    test_vectors into vectors of length vector_length. For example,
    vectorizing the test vectors

    [TestVector((10, 20), 30, tolerance), TestVector((11, 20), 31, tolerance)]

    into vectors of length 2 would produce the result:

    [TestVector((vec2(10, 11), vec2(20, 20)), vec2(30, 31), new_tolerance)].

    Tolerances are combined in root-sum-square fashion.

    scalar_arg_indices is a sequence of argument indices which should
    not be vectorized.  So, if scalar_arg_indices is [1] in the above
    example, the result would be:

    [TestVector((vec2(10, 11), 20), vec2(30, 31), new_tolerance)].
    """
    def make_groups(test_vectors):
	"""Group test vectors according to the values passed to the
	arguments that should not be vectorized.
	"""
	groups = {}
	for tv in test_vectors:
	    key = tuple(tv.arguments[i] for i in scalar_arg_indices)
	    if key not in groups:
		groups[key] = []
	    groups[key].append(tv)
	return groups
    def partition_vectors(test_vectors, partition_size):
	"""Partition test_vectors into lists of length partition_size.
	If partition_size does not evenly divide the number of test
	vectors, wrap around as necessary to ensure that every input
	test vector is included.
	"""
	for i in xrange(0, len(test_vectors), partition_size):
	    partition = []
	    for j in xrange(partition_size):
		partition.append(test_vectors[(i + j) % len(test_vectors)])
	    yield partition
    def merge_vectors(test_vectors):
	"""Merge the given set of test vectors (whose arguments and
	result are scalars) into a single test vector whose arguments
	and result are vectors.  For argument indices in
	scalar_arg_indices, leave the argument as a scalar.
	"""
	arity = len(test_vectors[0].arguments)
	arguments = []
	for j in xrange(arity):
	    if j in scalar_arg_indices:
		arguments.append(test_vectors[0].arguments[j])
	    else:
		arguments.append(
		    np.array([tv.arguments[j] for tv in test_vectors]))
	result = np.array([tv.result for tv in test_vectors])
	tolerance = np.float32(
	    np.linalg.norm([tv.tolerance for tv in test_vectors]))
	return TestVector(arguments, result, tolerance)
    vectorized_test_vectors = []
    groups = make_groups(test_vectors)
    for key in sorted(groups.keys()):
	test_vectors = groups[key]
	vectorized_test_vectors.extend(
	    merge_vectors(partition)
	    for partition in partition_vectors(test_vectors, vector_length))
    return vectorized_test_vectors



def _store_test_vector(test_suite_dict, name, glsl_version, test_vector):
    """Store a test vector in the appropriate place in
    test_suite_dict.  The dictionary key (which is a Signature tuple)
    is generated by consulting the argument and return types of the
    test vector, and combining them with name and glsl_version.

    glsl_version is adjusted if necessary to reflect when the argument
    and return types were introduced into GLSL.
    """
    rettype = glsl_type_of(test_vector.result)
    argtypes = tuple(glsl_type_of(arg) for arg in test_vector.arguments)
    adjusted_glsl_version = max(
	glsl_version, rettype.version_introduced,
	*[t.version_introduced for t in argtypes])
    signature = Signature(name, adjusted_glsl_version, rettype, argtypes)
    if signature not in test_suite_dict:
	test_suite_dict[signature] = []
    test_suite_dict[signature].append(test_vector)



def _store_test_vectors(test_suite_dict, name, glsl_version, test_vectors):
    """Store multiple test vectors in the appropriate places in
    test_suite_dict.
    """
    for test_vector in test_vectors:
	_store_test_vector(test_suite_dict, name, glsl_version, test_vector)



def make_arguments(input_generators):
    """Construct a list of tuples of input arguments to test.

    input_generators is a list, the ith element of which is a sequence
    of values that are suitable for use as the ith argument of the
    function under test.

    Output is a list, each element of which is a tuple of arguments to
    be passed to the function under test.  These values are produced
    by taking the cartesian product of the input sequences.

    In addition, this function rounds floating point inputs to 32
    bits, so that there will be no rounding errors when the input
    values are passed into OpenGL.
    """
    input_generators = [
	[round_to_32_bits(x) for x in seq] for seq in input_generators]
    return list(itertools.product(*input_generators))



def _make_componentwise_test_vectors(test_suite_dict):
    """Add test vectors to test_suite_dict for GLSL built-in
    functions that operate on vectors in componentwise fashion.
    Examples include sin(), cos(), min(), max(), and clamp().
    """
    def f(name, arity, glsl_version, python_equivalent,
	  alternate_scalar_arg_indices, test_inputs,
	  tolerance_function = _strict_tolerance):
	"""Create test vectors for the function with the given name
	and arity, which was introduced in the given glsl_version.

	python_equivalent is a Python function which operates on scalars,
	and simulates the GLSL function.  This function should return None
	in any case where the output of the GLSL function is undefined.

	If alternate_scalar_arg_indices is not None, also create test
	vectors for an alternate vectorized version of the function,
	in which some arguments are scalars.
	alternate_scalar_arg_indices is a sequence of the indices of
	the arguments which are scalars.

	test_inputs is a list, the ith element of which is a list of
	values that are suitable for use as the ith argument of the
	function.

	If tolerance_function is supplied, it is a function which
	should be used to compute the tolerance for the test vectors.
	Otherwise, _strict_tolerance is used.
	"""
	scalar_test_vectors = _simulate_function(
	    make_arguments(test_inputs), python_equivalent, tolerance_function)
	_store_test_vectors(
	    test_suite_dict, name, glsl_version, scalar_test_vectors)
	if alternate_scalar_arg_indices is None:
	    scalar_arg_indices_list = [()]
	else:
	    scalar_arg_indices_list = [(), alternate_scalar_arg_indices]
	for scalar_arg_indices in scalar_arg_indices_list:
	    for vector_length in (2, 3, 4):
		_store_test_vectors(
		    test_suite_dict, name, glsl_version,
		    _vectorize_test_vectors(
			scalar_test_vectors, scalar_arg_indices,
			vector_length))
    f('radians', 1, '1.10', np.radians, None, [np.linspace(-180.0, 180.0, 4)])
    f('degrees', 1, '1.10', np.degrees, None, [np.linspace(-np.pi, np.pi, 4)])
    f('sin', 1, '1.10', np.sin, None, [np.linspace(-np.pi, np.pi, 4)], _trig_tolerance)
    f('cos', 1, '1.10', np.cos, None, [np.linspace(-np.pi, np.pi, 4)], _trig_tolerance)
    f('tan', 1, '1.10', np.tan, None, [np.linspace(-np.pi, np.pi, 4)], _trig_tolerance)
    f('asin', 1, '1.10', np.arcsin, None, [np.linspace(-1.0, 1.0, 4)], _trig_tolerance)
    f('acos', 1, '1.10', np.arccos, None, [np.linspace(-1.0, 1.0, 4)], _trig_tolerance)
    f('atan', 1, '1.10', np.arctan, None, [np.linspace(-2.0, 2.0, 4)], _trig_tolerance)
    f('atan', 2, '1.10', _arctan2, None, [np.linspace(-2.0, 2.0, 3), np.linspace(-2.0, 2.0, 3)], _trig_tolerance)
    f('pow', 2, '1.10', _pow, None, [np.linspace(0.0, 2.0, 4), np.linspace(-2.0, 2.0, 4)])
    f('exp', 1, '1.10', np.exp, None, [np.linspace(-2.0, 2.0, 4)])
    f('log', 1, '1.10', np.log, None, [np.linspace(0.01, 2.0, 4)])
    f('exp2', 1, '1.10', np.exp2, None, [np.linspace(-2.0, 2.0, 4)])
    f('log2', 1, '1.10', np.log2, None, [np.linspace(0.01, 2.0, 4)])
    f('sqrt', 1, '1.10', np.sqrt, None, [np.linspace(0.0, 2.0, 4)])
    f('inversesqrt', 1, '1.10', lambda x: 1.0/np.sqrt(x), None, [np.linspace(0.1, 2.0, 4)])
    f('abs', 1, '1.10', np.abs, None, [np.linspace(-1.5, 1.5, 5)])
    f('sign', 1, '1.10', np.sign, None, [np.linspace(-1.5, 1.5, 5)])
    f('floor', 1, '1.10', np.floor, None, [np.linspace(-2.0, 2.0, 4)])
    f('ceil', 1, '1.10', np.ceil, None, [np.linspace(-2.0, 2.0, 4)])
    f('fract', 1, '1.10', lambda x: x-np.floor(x), None, [np.linspace(-2.0, 2.0, 4)])
    f('mod', 2, '1.10', lambda x, y: x-y*np.floor(x/y), [1], [np.linspace(-1.9, 1.9, 4), np.linspace(-2.0, 2.0, 4)])
    f('min', 2, '1.10', min, [1], [np.linspace(-2.0, 2.0, 4), np.linspace(-2.0, 2.0, 4)])
    f('max', 2, '1.10', max, [1], [np.linspace(-2.0, 2.0, 4), np.linspace(-2.0, 2.0, 4)])
    f('clamp', 3, '1.10', _clamp, [1, 2], [np.linspace(-2.0, 2.0, 4), np.linspace(-1.5, 1.5, 3), np.linspace(-1.5, 1.5, 3)])
    f('mix', 3, '1.10', lambda x, y, a: x*(1-a)+y*a, [2], [np.linspace(-2.0, 2.0, 2), np.linspace(-3.0, 3.0, 2), np.linspace(0.0, 1.0, 4)])
    f('step', 2, '1.10', lambda edge, x: 0.0 if x < edge else 1.0, [0], [np.linspace(-2.0, 2.0, 4), np.linspace(-2.0, 2.0, 4)])
    f('smoothstep', 3, '1.10', _smoothstep, [0, 1], [np.linspace(-1.9, 1.9, 4), np.linspace(-1.9, 1.9, 4), np.linspace(-2.0, 2.0, 4)])
_make_componentwise_test_vectors(test_suite)



def _make_vector_relational_test_vectors(test_suite_dict):
    """Add test vectors to test_suite_dict for GLSL built-in functions
    that operate on vectors of floats, ints, or bools, but not on
    single floats, ints, or bools.  Examples include lessThan(),
    equal(), and not().
    """
    _default_inputs = {
	'v': np.linspace(-1.5, 1.5, 4),
	'i': np.array([1, 2, 3, 4]),
	'b': np.array([False, True])
	}
    def f(name, arity, glsl_version, python_equivalent, arg_types,
	  tolerance_function = _strict_tolerance):
	"""Make test vectors for the function with the given name and
	arity, which was introduced in the given glsl_version.

	python_equivalent is a Python function which operates on scalars,
	and simulates the GLSL function.

	arg_types is a string containing 'v' if the function supports
	standard "vec" inputs, 'i' if it supports "ivec" inputs, and 'b'
	if it supports "bvec" inputs.  The output type of the function is
	assumed to be the same as its input type.

	If tolerance_function is supplied, it is a function which
	should be used to compute the tolerance for the test vectors.
	Otherwise, _strict_tolerance is used.
	"""
	for arg_type in arg_types:
	    test_inputs = [_default_inputs[arg_type]]*arity
	    scalar_test_vectors = _simulate_function(
		make_arguments(test_inputs), python_equivalent,
		tolerance_function)
	    for vector_length in (2, 3, 4):
		_store_test_vectors(
		    test_suite_dict, name, glsl_version,
		    _vectorize_test_vectors(
			scalar_test_vectors, (), vector_length))
    f('lessThan', 2, '1.10', lambda x, y: x < y, 'vi')
    f('lessThanEqual', 2, '1.10', lambda x, y: x <= y, 'vi')
    f('greaterThan', 2, '1.10', lambda x, y: x > y, 'vi')
    f('greaterThanEqual', 2, '1.10', lambda x, y: x >= y, 'vi')
    f('equal', 2, '1.10', lambda x, y: x == y, 'vib')
    f('not', 1, '1.10', lambda x: not x, 'b')
_make_vector_relational_test_vectors(test_suite)



def _make_vector_or_matrix_test_vectors(test_suite_dict):
    """Add test vectors to test_suite_dict for GLSL built-in functions
    that operate on vectors/matrices as a whole.  Examples include
    length(), dot(), cross(), normalize(), and refract().
    """
    _std_vectors = [
	-1.33,
	 0.85,
	 np.array([-0.10, -1.20]),
	 np.array([-0.42, 0.48]),
	 np.array([-0.03, -0.85, -0.94]),
	 np.array([1.67, 0.66, 1.87]),
	 np.array([-1.65, 1.33, 1.93, 0.76]),
	 np.array([0.80, -0.15, -0.51, 0.0])
	 ]
    _std_vectors3 = [
	np.array([-0.03, -0.85, -0.94]),
	np.array([1.67, 0.66, 1.87]),
	]
    _normalized_vectors = [_normalize(x) for x in _std_vectors]
    _nontrivial_vectors = [x for x in _std_vectors if not isinstance(x, FLOATING_TYPES)]
    _std_matrices = [
	np.array([[ 1.60,  0.76],
		  [ 1.53, -1.00]]), # mat2
	np.array([[-0.13, -0.87],
		  [-1.40,  1.40]]), # mat2
	np.array([[-1.11,  1.67, -0.41],
		  [ 0.13,  1.09, -0.02],
		  [ 0.56,  0.95,  0.24]]), # mat3
	np.array([[-1.69, -0.46, -0.18],
		  [-1.09,  1.75,  2.00],
		  [-1.53, -0.70, -1.47]]), # mat3
	np.array([[-1.00, -0.55, -1.08,  1.79],
		  [ 1.77,  0.62,  0.48, -1.35],
		  [ 0.09, -0.71, -1.39, -1.21],
		  [-0.91, -1.82, -1.43,  0.72]]), # mat4
	np.array([[ 0.06,  1.31,  1.52, -1.96],
		  [ 1.60, -0.32,  0.51, -1.84],
		  [ 1.25,  0.45,  1.90, -0.72],
		  [-0.16,  0.45, -0.88,  0.39]]), # mat4
	np.array([[ 0.09,  1.30,  1.25],
		  [-1.19,  0.08,  1.08]]), # mat3x2
	np.array([[-0.36, -1.08, -0.60],
		  [-0.53,  0.88, -1.79]]), # mat3x2
	np.array([[-0.46,  1.94],
		  [-0.45, -0.75],
		  [ 1.03, -0.50]]), # mat2x3
	np.array([[ 1.38, -1.08],
		  [-1.27,  1.83],
		  [ 1.00, -0.74]]), # mat2x3
	np.array([[ 1.81, -0.87,  0.81,  0.65],
		  [-1.16, -1.52,  0.25, -1.51]]), # mat4x2
	np.array([[ 1.93, -1.63,  0.29,  1.60],
		  [ 0.49,  0.27,  0.14,  0.94]]), # mat4x2
	np.array([[ 0.16, -1.69],
		  [-0.80,  0.59],
		  [-1.74, -1.43],
		  [-0.02, -1.21]]), # mat2x4
	np.array([[-1.02,  0.74],
		  [-1.64, -0.13],
		  [-1.59,  0.47],
		  [ 0.30,  1.13]]), # mat2x4
	np.array([[-0.27, -1.38, -1.41, -0.12],
		  [-0.17, -0.56,  1.47,  1.86],
		  [-1.85, -1.29,  1.77,  0.01]]), # mat4x3
	np.array([[-0.47, -0.15,  1.97, -1.05],
		  [-0.20,  0.53, -1.82, -1.41],
		  [-1.39, -0.19,  1.62,  1.58]]), # mat4x3
	np.array([[ 1.42, -0.86,  0.27],
		  [ 1.80, -1.74,  0.04],
		  [-1.88, -0.37,  0.43],
		  [ 1.37,  1.90,  0.71]]), # mat3x4
	np.array([[-1.72,  0.09,  0.45],
		  [-0.31, -1.58,  1.92],
		  [ 0.14,  0.18, -0.56],
		  [ 0.40, -0.77,  1.76]]), # mat3x4
	]
    _ft = [False, True]
    _bvecs = [np.array(bs) for bs in itertools.product(_ft, _ft)] + \
	[np.array(bs) for bs in itertools.product(_ft, _ft, _ft)] + \
	[np.array(bs) for bs in itertools.product(_ft, _ft, _ft, _ft)]
    def f(name, arity, glsl_version, python_equivalent,
	  argument_indices_to_match, test_inputs,
	  tolerance_function = _strict_tolerance):
	"""Make test vectors for the function with the given name and
	arity, which was introduced in the given glsl_version.

	python_equivalent is a Python function which simulates the GLSL
	function.  This function should return None in any case where the
	output of the GLSL function is undefined.  However, it need not
	check that the lengths of the input vectors are all the same.

	If argument_indices_to_match is not None, it is a sequence of
	argument indices indicating which arguments of the function
	need to have matching types.

	test_inputs is a list, the ith element of which is a list of
	vectors and/or scalars that are suitable for use as the ith
	argument of the function.

	If tolerance_function is supplied, it is a function which
	should be used to compute the tolerance for the test vectors.
	Otherwise, _strict_tolerance is used.
	"""
	test_inputs = make_arguments(test_inputs)
	if argument_indices_to_match is not None:
	    test_inputs = [
		arguments
		for arguments in test_inputs
		if _argument_types_match(arguments, argument_indices_to_match)]
	_store_test_vectors(
	    test_suite_dict, name, glsl_version,
	    _simulate_function(
		test_inputs, python_equivalent, tolerance_function))
    f('length', 1, '1.10', np.linalg.norm, None, [_std_vectors])
    f('distance', 2, '1.10', lambda x, y: np.linalg.norm(x-y), [0, 1], [_std_vectors, _std_vectors])
    f('dot', 2, '1.10', np.dot, [0, 1], [_std_vectors, _std_vectors])
    f('cross', 2, '1.10', np.cross, [0, 1], [_std_vectors3, _std_vectors3], _cross_product_tolerance)
    f('normalize', 1, '1.10', _normalize, None, [_std_vectors])
    f('faceforward', 3, '1.10', _faceforward, [0, 1, 2], [_std_vectors, _std_vectors, _std_vectors])
    f('reflect', 2, '1.10', _reflect, [0, 1], [_std_vectors, _normalized_vectors])
    f('refract', 3, '1.10', _refract, [0, 1], [_normalized_vectors, _normalized_vectors, [0.5, 2.0]])

    # Note: technically matrixCompMult operates componentwise.
    # However, since it is the only componentwise function to operate
    # on matrices, it is easier to generate test cases for it here
    # than to add matrix support to _make_componentwise_test_vectors.
    f('matrixCompMult', 2, '1.10', lambda x, y: x*y, [0, 1], [_std_matrices, _std_matrices])

    f('outerProduct', 2, '1.20', np.outer, None, [_nontrivial_vectors, _nontrivial_vectors])
    f('transpose', 1, '1.20', np.transpose, None, [_std_matrices])
    f('any', 1, '1.10', any, None, [_bvecs])
    f('all', 1, '1.10', all, None, [_bvecs])
_make_vector_or_matrix_test_vectors(test_suite)