.. highlight:: php
   :linenothreshold: 2

.. index::
   single: Propel

Bazy danych a Propel
====================

Propel is jest otwartą biblioteką Object-Relational Mapping (ORM) dla PHP,
która implementuje `wzorzec ActiveRecord`_. Zapewnia ona dostęp do bazy danych
przy uzyciu zestawu obiektów, dostarczając proste API dla zapisywania
i pobierania danych.
Propel wykorzystuje PDO jako wartwę abstrakcyjną i generowania kodu.

Kilka lat temu Propel był bardzo popularną alernatywa dla Doctrine. Jednak obecnie
jego populatność gwałtownie spadła. Dlatego dokumentacja Propel została usunięta
z podręcznika Symfony. Informacje o zastosowaniu Propel w Symfony są dostępne
w `oficjalnej dokumentacji pakietu PropelBundle`_.

.. _`wzorzec ActiveRecord`: https://en.wikipedia.org/wiki/Active_record_pattern
.. _`oficjalnej dokumentacji pakietu PropelBundle`: https://github.com/propelorm/PropelBundle/blob/1.4/Resources/doc/index.markdown
