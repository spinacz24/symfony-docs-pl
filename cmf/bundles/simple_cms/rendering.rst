.. index::
    single: renderowanie; SimpleCmsBundle

Renderowanie
------------

Można skonfigurować odwzorowanie klasy dokumentu do szablonu albo kontrolera
konfigurując :ref:`RoutingBundle <reference-config-routing-dynamic>`.
Gdy zachodzi potrzeba ustawienia specyficznych opcji dla pojedynczej strony,
można wywołać ``setDefault()`` dla klucza ``_template`` lub   domyślna wartość
``_controller`` w instancji strony.

Prosty przykład takiego szablonu może wyglądać tak:

.. configuration-block::

    .. code-block:: html+jinja

        {% block content -%}
            <h1>{{ page.title }}</h1>

            <div>{{ page.body|raw }}</div>

            <ul>
                {% for tag in page.tags -%}
                    <li>{{ tag }}</li>
                {%- endfor %}
            </ul>
        {%- endblock %}

    .. code-block:: html+php

        <?php $view['slots']->start('content') ?>
        <h1><?php $page->getTitle() ?></h1>

        <div><?php $page->getBody() ?></div>

        <ul>
        <?php foreach ($page->getTags() as $tag) : ?>
            <li><?php echo $tag ?></li>
        <?php endforeach ?>
        </ul>
        <?php $view['slots']->stop() ?>

Jeśli ma się włączony CreateBundle, można również wyprowadzić dokument z adnotacji
RDF, umożliwiając edytowanie treści, jak również stosowanie tagów we frontowej
części aplikacji. Najbardziej prostą postacią jest następujący blok Twig:

.. code-block:: jinja

    {% block content %}
        {% createphp page as="rdf" %}
            {{ rdf|raw }}
        {% endcreatephp %}
    {% endblock %}

Jeśli chcesz bardziej kontrolować, to co powinno być pokazywane przez adnotacje RDF,
przeczytaj rozdział :ref:`bundle-create-usage-embed`.
