from abc import ABC, abstractmethod
from typing import Dict


class EverestConfigAdjustmentStrategy(ABC):
    """ Strategy that manipulates a (parsed) EVerest config when called.

     Used to build up / adapt EVerest configurations for tests.

     Adjustments can be collected during the configuration setup process. The Everst core class then applies all
     configuration adjustments.
     """

    @abstractmethod
    def adjust_everest_configuration(self, config: Dict) -> Dict:
        """ Adjusts the provided configuration by making a (deep) copy and returning the adjusted configuration. """
        pass
