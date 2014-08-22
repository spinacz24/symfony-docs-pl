.. highlight:: php
   :linenothreshold: 2

.. index::
    single: szablonowanie; CoreBundle

Szablonowanie
-------------

Pakiet CoreBundle dostarcza szereg funkcji wykorzystywanych w szablonach.

Rozszerzenie Twig i helper PHP
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CoreBundle rejestruje rozszerzenie Twig i helper szablonowania PHP. Oba działają
w ten sam sposób i są razem udokumentowane. Zawsze listujemy zarówno nazwę funkcji
Twig jak i nazwy metody helpera ``cmf``.

Wiele metod współdzieli te same wspólne parametry:

* **ignoreRole**: Metody mające ten parametr domyślnie filtrują zestaw wyników,
  aby zawierały tylko dokumenty publikowane, wykorzystując
  :doc:`proces publikowania <publish_workflow>`. Można ustawić parametr na
  ``true`` w celu wyłączenia filtrowania, jeśli wie się jakie są tego skutki.
* **class**: Parametr klasy, który może zostać wykorzystany do filtrowania zestawu
  wyników przez usuniecie wszystkich dokumentów, które nie implementują określonej
  klasy. 
* **limit, offset**: Używany do dzielenia listy wyników na strony. Trzeba mieć na
  uwadze, że podział na strony ma miejsce przed filtrowaniem.

Podstawowe operacje repozytorium
................................

* **cmf_find|find(path)**: Pobiera dokumentu dla dostarczanej ścieżki lub null,
  jeśli nic nie znaleziono. Wykonywana jest kontrola procesu publikacji w zakresie
  nie publikowania.
* **cmf_find_many|findMany($paths, $limit = false, $offset = false, $ignoreRole = false, $class = false)**:
  Pobiera listę dokumentów dla dostarczanych ścieżek.
* **cmf_nodename|getNodeName($document)**: Pobiera nazwę węzła pobieranego dokumentu.
* **cmf_path|getPath($document)**: Pobiera ścieżkę dostarczanego dokumentu.
* **cmf_parent_path|getParentPath($document)**: Pobiera ścieżkę rodzica dostarczanego dokumentu.

Poruszanie się po drzewie PHPCR
...............................

+-----------------------+----------------------+----------------------+---------------------------------------------------------------------------+
| Funkcja Twign         | Helper szablonowania | Argumenty            | Wyjaśnienie                                                               |
+=======================+======================+======================+===========================================================================+
| cmf_prev              | getPrev              | $current,            | Pobiera poprzedni dokument tego samego rodzica z listy PHPCR. Jeśli       |
|                       |                      | $depth = null,       | jest ustawiony ``$anchor`` (zarówno dokumentu jak i ścieżki), to w        |
|                       |                      | $ignoreRole = false, | drzewie brane będą również pod uwagę elementy sąsiednie ``$current``.     |
|                       |                      | $class = null,       | Gdy ustawiony jest ``$depth``, to wyznacza on ograniczenie zagłębianie    |
|                       |                      | $anchor = null       | się w strukturę drzewa poniżej ``$current``.                              |
+-----------------------+----------------------+----------------------+---------------------------------------------------------------------------+
| cmf_prev_linkable     | getPrevLinkable      | $current,            | Pobiera poprzedni dokument, który może zostać zlinkowany, zgodnie         |
|                       |                      | $anchor = null,      | z niżej opisaną metodą ``isLinkable``.                                    |
|                       |                      | $depth = null,       |                                                                           |
|                       |                      | $ignoreRole = false  |                                                                           |
+-----------------------+----------------------+----------------------+---------------------------------------------------------------------------+
| cmf_next              | getNext              | $current,            | Pobiera następny dokument tego samego rodzica z ``$current``              |
|                       |                      | $anchor = null,      | (dokument lub ścieżkę) w drzewie PHPCR. Argumenty ``$anchor``             |
|                       |                      | $depth = null,       | i ``$depth`` mają ta samą semantykę jak w ``getPrev``.                    |
|                       |                      | $ignoreRole = false, |                                                                           |
|                       |                      | $class = null        |                                                                           |
+-----------------------+----------------------+----------------------+---------------------------------------------------------------------------+
| cmf_next_linkable     | getNextLinkable      | $current,            | Pobiera następny dokument, który może zostać zlinkowany, zgodnie          |
|                       |                      | $anchor = null,      | z niżej opisaną metodą ``isLinkable``.                                    |
|                       |                      | $depth = null,       |                                                                           |
|                       |                      | $ignoreRole = false  |                                                                           |
+-----------------------+----------------------+----------------------+---------------------------------------------------------------------------+
| cmf_child             | getChild             | $parent, $name       | Pobiera dokument podrzędny o nazwie ``$name`` określonego rodzica.        |
|                       |                      |                      | Rodzic może być określony przez identyfikator lub dokument. Wykonywana    |
|                       |                      |                      | jest kontrola procesu publikowania w zakresie nie publikowania.           |
+-----------------------+----------------------+----------------------+---------------------------------------------------------------------------+
| cmf_children          | getChildren          | $parent,             | Pobiera wszystkie dzieci tego samego rodzica, w kolejności występowania   |
|                       |                      | $limit = false,      | w PHPCR. Rodzic może być określony przez identyfikator lub dokument.      |
|                       |                      | $offset = false,     | *$filter jest obecnie ignorowny*.                                         |
|                       |                      | $filter = null,      |                                                                           |
|                       |                      | $ignoreRole = false, |                                                                           |
|                       |                      | $class = null        |                                                                           |
+-----------------------+----------------------+----------------------+---------------------------------------------------------------------------+
| cmf_linkable_children | getLinkableChildren  | $parent,             | Pobiera wszystkie dzieci ``$parent``, które mogą zostać zlikowane,        |
|                       |                      | $limit = false,      | zgodnie z niżej opisaną metodą ``isLinkable``.                            |
|                       |                      | $offset = false,     |                                                                           |
|                       |                      | $filter = null,      |                                                                           |
|                       |                      | $ignoreRole = false  |                                                                           |
+-----------------------+----------------------+----------------------+---------------------------------------------------------------------------+
| cmf_descendants       | getDescendants       | $parent,             | Pobiera wszystkie **ścieżki** potomków ``$parent`` (dokument lub ścieżka) |
|                       |                      | $depth = null        | w repozytorium. Argument ``$depth`` może zostać użyty do określenia       |
|                       |                      |                      | głębokości do jakiej wyszukiwani są potomkowie w strukturze drzewa.       |
|                       |                      |                      | Jeśli nie określono tego argumenty, to głębokość jest nieograniczona. Nie |
|                       |                      |                      | są podejmowane próby kontroli procesu publikowania, ponieważ dokumenty    |
|                       |                      |                      | nie są nawet załadowane.                                                  |
+-----------------------+----------------------+----------------------+---------------------------------------------------------------------------+


Metody helpera
..............

+----------------------+---------------+---------------------+---------------------------------------------------------------------------+
| cmf_document_locales | getLocalesFor | $document,          | Pobiera ustawienia regionalne dostarczanego dokumentu. Jeśli wartością    |
|                      |               | $includeFallbacks = | ``$includeFallbacks`` jest ``true``, to wszystkie awaryjne ustawienia     |
|                      |               | false               | regionalne są również dostarczane , nawet jeśli nie istnieje tłumaczenie  |
|                      |               |                     | danym języku.                                                             |
+----------------------+---------------+---------------------+---------------------------------------------------------------------------+
| cmf_is_linkable      | isLinkable    | $document           | Sprawdza, czy dostarczony obiekt może zostać użyty do generowania URL.    |
|                      |               |                     | Jeśli sprawdzenie to zwraca true, to należy go zapisać w celu przekazania |
|                      |               |                     | go do ``path`` lub ``url``. Obiekt jest uważany za zdolny do linkowania,  |
|                      |               |                     | jeśli jest instancją ``Route`` albo implementuje                          |
|                      |               |                     | ``RouteReferrersReadInterface`` i rzeczywiście zwraca trasę.              |
+----------------------+---------------+---------------------+---------------------------------------------------------------------------+
| cmf_is_published     | isPublished   | $document           | Sprawdza w procesie publikowania, czy dostarczany obiekt jest             |
|                      |               |                     | opublikowany. Zobacz dla przykładu                                        |
|                      |               |                     | :ref:`cmf_is_published <bundle-core-publish-workflow-twig_function>`.     |
+----------------------+---------------+---------------------+---------------------------------------------------------------------------+

Przykłady kodu
..............

.. configuration-block::

    .. code-block:: html+jinja
       :linenos:

        {% set page = cmf_find('/some/path') %}

        {% if cmf_is_published(page) %}
            {% set prev = cmf_prev_linkable(page) %}
            {% if prev %}
                <a href="{{ path(prev) }}">prev</a>
            {% endif %}

            {% set next = cmf_next_linkable(page) %}
            {% if next %}
                <span style="float: right; padding-right: 40px;"><a href="{{ path(next) }}">next</a></span>
            {%  endif %}

            {% for news in cmf_children(parent=cmfMainContent, class='Acme\\DemoBundle\\Document\\NewsItem')|reverse %}
                <li><a href="{{ path(news) }}">{{ news.title }}</a> ({{ news.publishStartDate | date('Y-m-d')  }})</li>
            {% endfor %}

            {% if 'de' in cmf_document_locales(page) %}
                <a href="{{ path(
                    app.request.attributes.get('_route'),
                    app.request.attributes.get('_route_params')|merge(app.request.query.all)|merge({
                        '_locale': 'de'
                    })
                ) }}">DE</a>
            {%  endif %}
            {% if 'fr' in cmf_document_locales(page) %}
                <a href="{{ path(
                    app.request.attributes.get('_route'),
                    app.request.attributes.get('_route_params')|merge(app.request.query.all)|merge({
                        '_locale': 'fr'
                    })
                ) }}">FR</a>
            {% endif %}
        {% endif %}

    .. code-block:: html+php
       :linenos:

        <?php $page = $view['cmf']->find('/some/path') ?>

        <?php if $view['cmf']->isPublished($page) : ?>
            <?php $prev = $view['cmf']->getPrev($page) ?>
            <?php if ($prev) : ?>
                <a href="<?php echo $view['router']->generate($prev) ?>">prev</a>
            <?php endif ?>

            <?php $next = $view['cmf']->getNext($page) ?>
            <?php if ($next) : ?>
                <span style="float: right; padding-right: 40px;">
                    <a href="<?php echo $view['router']->generate($next) ?>">next</a>
                </span>
            <?php endif ?>

            <?php foreach (array_reverse($view['cmf']->getChildren($page)) as $news) : ?>
                <li>
                    <a href="<?php echo $view['router']->generate($news) ?>"><?php echo $news->getTitle() ?></a>
                    (<?php echo date('Y-m-d', $news->getPublishStartDate()) ?>)
                </li>
            <?php endforeach ?>

            <?php if (in_array('de', $view['cmf']->getLocalesFor($page))) : ?>
                <a href="<?php $view['router']->generate
                    $app->getRequest()->attributes->get('_route'),
                    array_merge(
                        $app->getRequest()->attributes->get('_route_params'),
                        array_merge(
                            $app->getRequest()->query->all(),
                            array('_locale' => 'de')
                        )
                    )
                ?>">DE</a>
            <?php endif ?>
            <?php if (in_array('fr', $view['cmf']->getLocalesFor($page))) : ?>
                <a href="<?php $view['router']->generate
                    $app->getRequest()->attributes->get('_route'),
                    array_merge(
                        $app->getRequest()->attributes->get('_route_params'),
                        array_merge(
                            $app->getRequest()->query->all(),
                            array('_locale' => 'fr')
                        )
                    )
                ?>">FR</a>
            <?php endif ?>
        <?php endif ?>

.. tip::

    Podczas używania argumentu ``class`` należy pamiętać, że Twig będzie *ignorował*
    pojedyncze lewe ukośniki. Jeśli chce się napisać ``Acme\DemoBundle\Document\NewsItem``,
    spowoduje to, że CMF będzie poszukiwał klasy AcmeDemoBundleDocumentNewsItem,
    co da w rezultacie pustą listę. To co trzeba napisać w szablonie, to
    ``Acme\\DemoBundle\\Document\\NewsItem``.
