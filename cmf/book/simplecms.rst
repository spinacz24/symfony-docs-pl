.. highlight:: php
   :linenothreshold: 2

.. index::
    single: SimpleCms

Pierwsze spojrzenie na wnętrze
==============================

W większości przypadków zastosowań CMS najbardziej podstawową potrzebą jest powiązanie
treści ze ścieżką URL.
W Symfony CMF odbywa się to za pomocą zaawansowanego systemu trasowania, dostarczanego
przez pakiety RoutingBundle i ContentBundle. Pakiet RoutingBundle zapewnia obiekt
``Route``, który można powiązac z obiektem ``Content`` pakietu ContentBundle.

Posiadanie dwóch obiektów jest najbardziej elastycznym rozwiązaniem. Dla tej samej
treści można mieć różne trasy (np. oddzielnie dla każdego języka) lub można zorganizować
swoje treści odmiennie niż w drzewie URL. Jednak w wielu sytuacjach wystarczy posiadanie
jednej trasy dla jednej treści, co znacznie upraszcza stan rzeczy. To jest dokładnie
to samo, co wykonuje pakiet SimpleCmsBundle, który jest używany przez Symfony CMF
Standard Edition dla domyślnego trasowania, treści i menu.

.. note::
   
   Ważne jest aby zrozumieć, że pakiet SimpleCmsBundle ijest tylko prostym przykładem
   tego, jak można łączyć pakiety CMF w kompletny CMS. Zapraszamy do rozszerzania
   SimpleCmsBundle lub tworzenia własnych pakietów  wykonujących te zadania.

.. tip::
   
   W celu uzyskania więcej inforamcji o trasowaniu, proszę przeczytać ":doc:`routing`".
   Więcej informacji o przechowywaniu danych można znaleźć w ":doc:`static_content`".
   W końcu, aby poznać szczegółu tworzenia menu, proszę przeczytać
   ":doc:`structuring_content`".

Dokument strony
---------------

SimpleCmsBundle dostarcza klasę o nazwie ``Page``, która rozszerza rdzenną klasę
``Route`` oraz dostarcza właściwości do przechowywania treści. Pakiet ten
implementuje też ``NodeInterface``, dzięki czemu może być wykorzystywany w menu.
To podejście "trzy w jednym" jest kluczową koncepcją pakietu SimpleCmsBundle.

Odwzorowywanie ``Page`` do szablonu i kontrolera działa tak, jak wyjaśniono to w 
:doc:`następnym rozdziale <routing>`.

Tworzenie nowej strony
----------------------

W celu utworzenia strony, użyj obiektu
``Symfony\Cmf\Bundle\SimpleCmsBundle\Doctrine\Phpcr\Page``::

    // // src/Acme/MainBundle/DataFixtures/PHPCR/LoadSimpleCms.php
    namespace Acme\DemoBundle\DataFixtures\PHPCR;

    use Symfony\Cmf\Bundle\SimpleCmsBundle\Doctrine\Phpcr\Page;
    use Doctrine\ODM\PHPCR\DocumentManager;

    class LoadSimpleCms implements FixtureInterface
    {
        /**
         * @param DocumentManager $dm
         */
        public function load(ObjectManager $dm)
        {
            $parent = $dm->find(null, '/cms/simple');
            $page = new Page();
            $page->setTitle('About Symfony CMF');
            $page->setLabel('About');
            $page->setBody(...);

            // pozycja drzewa określa ścieżkę URL
            $page->setPosition($parent, 'about');

            $dm->persist($page);
            $dm->flush();
        }
    }

W obiekcie Page można również ustawić inne opcje (np. tags).

Wszystkie strony są przechowywane w prostej strukturze drzewa. W celu ustawienia
pozycji, trzeba użyć ``setPosition``. Pierwszy argument jest dokumentem nadrzędnym, drugi
nazwą strony. Nazwy te są wykorzystywane w ścieżce URL. Na przykład, można mieć
następująca strukturę drzewa:

.. code-block:: text
   :linenos:

    /cms/simple/
        about/
        blog/
            symfony-cmf-is-great/

W tym przypadku mamy 4 strony: ``/cms/simple``, ``about``, ``blog`` i
``symfony-cmf-is-great``.
Strona domowa ma ścieżkę ``/``. Strona ``symfony-cmf-is-great`` jest jest stroną
podrzędną ``blog`` i tym samym ma ścieżkę ``/blog/symfony-cmf-is-great``.
Dla utworzenia takiej struktury zrób to::

    // // src/Acme/MainBundle/DataFixtures/PHPCR/LoadSimpleCms.php
    namespace Acme\DemoBundle\DataFixtures\PHPCR;

    use Symfony\Cmf\Bundle\SimpleCmsBundle\Doctrine\Phpcr\Page;
    use Doctrine\ODM\PHPCR\DocumentManager;

    class LoadSimpleCms implements FixtureInterface
    {
        /**
         * @param DocumentManager $dm
         */
        public function load(ObjectManager $dm)
        {
            $root = $dm->find(null, '/cms/simple');

            $about = new Page();
            // ... set up about
            $about->setPosition($root, 'about');

            $dm->persist($about);

            $blog = new Page();
            // ... set up blog
            $blog->setPosition($root, 'blog');

            $dm->persist($blog);

            $blogPost = new Page();
            // ... set up blog post
            $blogPost->setPosition($blog, 'symfony-cmf-is-great');

            $dm->persist($blogPost);

            $dm->flush();
        }
    }

Każdy dokument PHPCR-ODM musi mieć dokument nadrzędny. Dokumenty nadrzędne nigdy
nie są tworzone automatycznie, więc wykorzystujemy PHPCR NodeHelper do zapewnienia
sobie elementu głównego (w tym przypadku ``/cms/simple``).

.. note::

    Strona na ``/cms/simple`` jest tworzona przez :ref:`inicjalizator <phpcr-odm-repository-initializers>`
    pakietu SimpleCmsBundle.

Podsumowanie
------------

Jesteś już w stanie stworzyć prostą witrynę internetową przy wykorzystaniu Symfony
CMF. Od tego miejsca, każdy następny rozdział przyniesie troche więcej informacji
o CMF i wyjaśni więcej rzeczy związanych z pakietem SimpleCMSBundle. Na koniec
będziesz w stanie stworzyć bardziej zaawansowany system blogu i witryn internetowych
innych odmian CMS.
