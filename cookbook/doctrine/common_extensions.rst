Rozszerzenia Doctrine: Timestampable, Sluggable, Translatable, itd.
===================================================================

Biblioteka Doctrine2 jest bardzo elastyczna, jej społeczność stworzyła już serię 
użytecznych rozszerzeń które pomogają w rozwiązaniu powszechnych zadań związanych z encjami.

W szczególności, jedna z tych bibliotek - `DoctrineExtensions`_ - dostarcza możliwość
zintegrowania z zachowaniami `Sluggable`_, `Translatable`_, `Timestampable`_,
`Loggable`_, `Tree`_ i `Sortable`_.

Zastosowanie każdego z tych rozszerzeń wyjaśniono w jego repozytorium.

Jednak, aby zainstalować i aktywować każde rozszerzenie, trzeba zarejestrować
i aktywować :doc:`detektor zdarzeń </cookbook/doctrine/event_listeners_subscribers>`.
Zrobić to można na dwa sposoby:

#. Użyć `StofDoctrineExtensionsBundle`_, który integruje powyższą bibliotekę.

#. Zaimplementować bezpośrednio usługi postępując zgodnie z dokumentacją dla
   integracji z Symfony: `Install Gedmo Doctrine2 extensions in Symfony2`_

.. _`DoctrineExtensions`: https://github.com/Atlantic18/DoctrineExtensions
.. _`StofDoctrineExtensionsBundle`: https://github.com/stof/StofDoctrineExtensionsBundle
.. _`Sluggable`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/sluggable.md
.. _`Translatable`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/translatable.md
.. _`Timestampable`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/timestampable.md
.. _`Loggable`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/loggable.md
.. _`Tree`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/tree.md
.. _`Sortable`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/sortable.md
.. _`Install Gedmo Doctrine2 extensions in Symfony2`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/symfony2.md
