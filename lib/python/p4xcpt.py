#
#@BEGIN LICENSE
#
# PSI4: an ab initio quantum chemistry software package
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
#@END LICENSE
#

"""Module with non-generic exceptions classes."""
from __future__ import absolute_import
import psi4


class PsiException(Exception):
    """Error class for Psi."""
    pass


class ValidationError(PsiException):
    """Error called for problems with the input file. Prints
    error message *msg* to standard output stream and output file.

    """
    def __init__(self, msg):
        PsiException.__init__(self, msg)
        self.msg = msg
        psi4.print_out('\nPsiException: %s\n\n' % (msg))


class TestComparisonError(PsiException):
    """Error called when a test case fails due to a failed
    compare_values() call. Prints error message *msg* to standard
    output stream and output file.

    """
    def __init__(self, msg):
        PsiException.__init__(self, msg)
        self.msg = msg
        psi4.print_out('\nPsiException: %s\n\n' % (msg))


class CSXError(PsiException):
    """Error called when CSX generation fails.

    """
    def __init__(self, msg):
        PsiException.__init__(self, msg)
        self.msg = msg
        psi4.print_out('\nCSXException: %s\n\n' % (msg))
