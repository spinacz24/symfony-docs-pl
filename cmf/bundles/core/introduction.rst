.. index::
    single: rdzeń CMF; pakiety 

CoreBundle
==========

    Pakiet ten dostarcza dla innych pakietów CMF wspólne funkcjonalności, helpery i narzędzia .

Jedną z dostarczanych funkcjonalności jest interfejs i implementacja weryfikatora
algorytmu publikowania z dołączonym interfejsem. Umożliwia on wybór, czy
w modelach ma być implementowana obsługa tego weryfikatora.

Ponadto dostarcza on helper Twiga wystawiający kilka przydatnych funkcji dla szablonów
Twiga do interakcji z dokumentami PHPCR-ODM.

Na koniec, większość ustawień konfiguracyjnych jest automatycznie stosowana jako
ustawienia domyślne we większości innych pakietów CMF.

Instalacja
----------

Pakiet ten można zainstalować `poprzez Composer`_ wykorzystując pakiet
`symfony-cmf/core-bundle`_.

Rozdziały
---------

* :doc:`publish_workflow`
* :doc:`dependency_injection_tags`
* :doc:`templating`
* :doc:`persistence`

.. _`symfony-cmf/core-bundle`: https://packagist.org/packages/symfony-cmf/core-bundle
.. _`poprzez Composer`: http://getcomposer.org
