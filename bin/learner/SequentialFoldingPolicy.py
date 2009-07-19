##############################################################################
# This file is part of pysgpp, a program package making use of spatially    #
# adaptive sparse grids to solve numerical problems                         #
#                                                                           #
# Copyright (C) 2009 Valeriy Khakhutskyy (khakhutv@in.tum.de)               #
#                                                                           #
# pysgpp is free software; you can redistribute it and/or modify            #
# it under the terms of the GNU General Public License as published by      #
# the Free Software Foundation; either version 3 of the License, or         #
# (at your option) any later version.                                       #
#                                                                           #
# pysgpp is distributed in the hope that it will be useful,                 #
# but WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
# GNU Lesser General Public License for more details.                       #
#                                                                           #
# You should have received a copy of the GNU General Public License         #
# along with pysgpp; if not, write to the Free Software                     #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA #
# or see <http://www.gnu.org/licenses/>.                                    #
#############################################################################

## @package SequentialFoldingPolicy
# @ingroup learner
# @brief Folding rule
# @version $CURR$

import time
import math

from FoldingPolicy import FoldingPolicy
from bin.data.ARFFAdapter import ARFFAdapter

#FIXME: this implementation is different from the RandomFoldingPolicy, since there no dataFold in the last one, check which on is correct
class SequentialFoldingPolicy(FoldingPolicy):
    
    
    ##Constructor
    #
    #@param dataset: DataContainer with data set
    #@param level: Integer folding level, default value: 1
    def __init__(self, dataContainer, level):
        FoldingPolicy.__init__(self,  dataContainer, level)
        self.seq = range(self.size)
        for step in xrange(self.level):
            validationIndeces = self.seq[ step * self.window : min((step+1) * self.window, self.size)]
            self.dataFold.append(self.createFoldsets(dataContainer, validationIndeces))
            
            
