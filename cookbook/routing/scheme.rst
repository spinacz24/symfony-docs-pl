.. index::
   single: trasowanie; wymóg schematu

Jak wymuszać trasy z protokołem HTTPS lub HTTP
==============================================

Czasami zachodzi potrzeba zabezpieczenia niektórych tras tak, aby zawsze były
dostępne tylko przez protokół HTTPS. Komponent Routing umożliwia wymuszenia
schematu URI poprzez ustawienie opcji ``_scheme``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        secure:
            path:     /secure
            defaults: { _controller: AppBundle:Main:secure }
            schemes:  [https]

    .. code-block:: xml
       :linenos:

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="secure" path="/secure" schemes="https">
                <default key="_controller">AppBundle:Main:secure</default>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('secure', new Route('/secure', array(
            '_controller' => 'AppBundle:Main:secure',
        ), array(), array(), '', array('https')));

        return $collection;

Powyższa konfiguracja wymusza na trasie ``secure``, aby zawsze używała HTTPS.

Podczas generowania adresu URL na trasie ``secure``, nawet jeśli aktualnm protokołem
będzie HTTP, Symfony automatycznie wygeneruje absolutny URL ze schematem HTTPS:

.. code-block:: twig
   :linenos:

    {# If the current scheme is HTTPS #}
    {{ path('secure') }}
    {# generates /secure #}

    {# If the current scheme is HTTP #}
    {{ path('secure') }}
    {# generates https://example.com/secure #}

Wymmaganie to jest także egzekwowane w stosunku do przychodzących żądań.
Jeśli uzytkownik bedzie próbował uzyskac dostęp do ``/secure`` poprzez HTTP,
zostaniesz automatycznie przekierowany pod ten sam adres ale ze schematem HTTPS.

W powyższym przykładzie użyliśmy w opcji ``_scheme`` wartość ``https``, ale można
także wymusić adres URL ze schematem ``http``.

.. note::

    Komponent Security zapewnia także inny sposób wymuszenia stosowania schematu
    HTTP lub HTTPS poprzez ustawienie opcji ``requires_channel``. Alternatywa ta
    jest lepsza dla zabezpieczenia części witryny (np. wszystkich ściezek URL
    rozpoczynających sie od ``/admin``) lub też kiedy chce się zabezpieczyć sciezki
    URL zdefiniowane w obcych pakietach (patrz :doc:`/cookbook/security/force_https`).
