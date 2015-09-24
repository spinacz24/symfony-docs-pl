payload
~~~~~~~

**typ**: ``mixed`` **domyślnie**: ``null``

.. versionadded:: 2.6
    Opcja ``payload`` została wprowadzona w Symfony 2.6.

Opcja ta moze być zastosowana do dołączenia do ograniczenia dowolnych danych
specyficznych dla domeny.
Skonfigurowna opcja ``payload`` nie jest wykorzystywana przez komponent Validator,
ale udostęþnione jej przetwarzania.

Na przykład, można potrzebować zastosowania 
:doc:`różnych poziomów błedów </cookbook/validation/severity>` w celu zróżnicowanego
prezentowania niespełnionych ograniczeń na stronie frontowej w zalezności od błędu.
