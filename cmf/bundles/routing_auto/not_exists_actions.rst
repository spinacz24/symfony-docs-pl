.. highlight:: php
   :linenothreshold: 2

.. index::
    single: RoutingAutoBundle; akcje nie istniejącej ścieżki

Akcje nie istniejącej ścieżki
-----------------------------

Sa to domyślne akcje wykonywane jeśli ścieżka dostarczona przez ``path_provider``
nie istnieje.

Akcja ``create``
~~~~~~~~~~~~~~~~

Akcja ``create`` tworzy ścieżkę.

.. configuration-block::

    .. code-block:: yaml

        not_exists_action: create

    .. code-block:: xml

        <not-exists-action name="create" />

    .. code-block:: php

        array(
            // ...
            'not_exists_action' => 'create',
        );

.. note::

   **Obecnie** wszystkie trasy dostarczane przez ścieżkę treści budowniczego
   jednostek będą tworzone jako dokumenty ``Generic``, podczas gdy trasy nazw
   treści będą tworzona jako dokumenty ``AutoRoute``.

Akcja ``throw_exception``
~~~~~~~~~~~~~~~~~~~~~~~~~

Akcja ta będzie zgłaszać wyjątek, jeśli trasa dostarczana przez dostawcę ścieżek
nie istnieje. Akcją tą powinno się zastosować, jeśli jest się pewnym, że trasa
*powinna* istnieć.

.. configuration-block::

    .. code-block:: yaml

        not_exists_action: throw_exception

    .. code-block:: xml

        <not-exists-action name="throw_exception" />

    .. code-block:: php

        array(
            // ...
            'not_exists_action' => 'throw_exception',
        );
