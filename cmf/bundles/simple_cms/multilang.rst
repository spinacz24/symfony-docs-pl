.. index::
    single: wielojezyczność; SimpleCmsBundle

Obsługa wielojęzyczności
------------------------

Ustawiając opcję ``add_locale_pattern`` na ``true`` w
``Symfony\Cmf\Bundle\SimpleCmsBundle\Doctrine\Phpcr\Page`` spowoduje to, że w łańcuchu
ścieżki dokument będzie poprzedzany oznaczeniem języka, ``/{_locale}``. Wykorzystując
natywne możliwości translacyjne PHPCR ODM możliwe jest utworzenie różnych wersji
językowych dokumentu, które powinny być dostępne na witrynie.

Na przykład::

    use Symfony\Cmf\Bundle\SimpleCmsBundle\Doctrine\Phpcr\Page;

    // ...

    // pass add_locale_pattern as true to prefix the route pattern with /{_locale}
    $page = new Page(array(add_locale_pattern => true));

    $page->setPosition($parent, 'hello-world');
    $page->setTitle('Hello World!');
    $page->setBody('Really interesting stuff...');
    $page->setLabel('Hello World');

    $dm->persist($page);
    $dm->bindTranslation($page, 'en');

    $page->setTitle('Hallo Welt!');
    $page->setBody('Super interessante Sachen...');
    $page->setLabel('Hallo Welt!');

    $dm->bindTranslation($page, 'de');

    $dm->flush();

.. sidebar:: Tłumaczenie adresów URL

    Ponieważ SimpleCmsBundle zapewnia tylko jedną strukturę drzewa, wszystkie węzły
    będą mieć tą samą nazwę we wszystkich językach. Tak więc adres url
    ``http://foo.com/en/hello-world`` dla treści w języku angielskim będzie wyglądać
    tak samo jak dla treści na przykład w języku niemieckim ``http://foo.com/de/hello-world``.

    Jeśli zachodzi potrzeba używania adresów URL specyficznych dla jakiegoś języka,
    można dodać dokumenty Route dla innych ustawień regionalnych, albo skonfigurować
    dynamiczny router do wyszukiwania tras z przedrostkami. Można również skompletować
    oddzielną trasę i treść używając oddzielnych dokumentów z pakietów RoutingBundle
    i ContentBundle.
