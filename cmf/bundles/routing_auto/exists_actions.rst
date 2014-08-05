.. index::
    single: RoutingAutoBundle; akcje istniejącej ścieżki

Akcje istniejącej ścieżki
-------------------------

Są to domyślne akcje rozwiązujące konflikt powstajacy przy próbie utworzenia
ścieżki dostarczaneja przez ``path_provider`` w systuacji, gdy taka ścieżka już
istnieje.

Akcja ``auto_increment``
~~~~~~~~~~~~~~~~~~~~~~~~

Akcja ``auto_increment`` dodaje do ścieżki numeryczny przyrostek.
Na przykład, ``my/path`` zostanie przekształcona na ``my/path-1`` i jeśli taka
ścieżka już istnieje, to podjęta będzie próba przekształcenia na ``my/path-2``,
``my/path-3`` i taj dalej, aż znajdzie się ścieżka jeszcze nie istniejąca.

Zwykle, akcja ta powinna zostać użyta w budowniczym jednostek ``content_name``,
aby rozwiązywać konflikty. Używanie jej w budowniczym łańcucha ``content_path``
nie ma sensu.

.. configuration-block::

    .. code-block:: yaml

        exists_action: auto_increment

    .. code-block:: xml

        <exists-action name="auto_increment" />

    .. code-block:: php

        array(
            // ...
            'exists_action' => 'auto_increment',
        );

Akcja ``use``
~~~~~~~~~~~~~

Akcja ``use`` po prostu pobiera istniejąca ścieżkę i ja stosuje. Na przykład,
w aplikacji forum budowniczy jednostek musi najpierw określić ścieżkę kategorii,
``/my-category``, jeśli ta ścieżka istnieje (a powinna), to następnie w stosie
zostanie ta akcja użyta.

Akcja ta zazwyczaj powinna być używana w jednej ze ścieżek treści budowniczego
jednostek, w celu czy powinna zostać użyta istniejąca trasa. Z drugiej strony,
używanie tego jako akcji budowniczego nazwy treści powinno spowodować, że starsza
trasa zostanie nadpisana.

.. configuration-block::

    .. code-block:: yaml

        exists_action: use

    .. code-block:: xml

        <exists-action name="use" />

    .. code-block:: php

        array(
            // ...
            'exists_action' => 'use',
        );
