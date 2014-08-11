.. index::
    single: tagi wstrzykiwania zależności; CoreBundle

Tagi wstrzykiwania zależności
-----------------------------

Tag cmf_request_aware
~~~~~~~~~~~~~~~~~~~~~

.. caution::

    Tag ten został zdeprecjonowany w CoreBundle 1.1 i zostanie usunięty
    w CoreBundle 1.2. Począwszy od Symfony 2.3 można wykorzystać fakt,
    że żądanie jest `usługą synchronizowaną`_.

Gdy pracuje się z wersją 1.0 CMF w Symfony 2.2 i ma się usługi, które wymagają
żądania (np. dla procesu publikowania lub wyborców pozycji bieżącego menu),
można oflagować usługi tagiem ``cmf_request_aware``, aby otrzymać wstrzykiwanie
rdzennego odbiornika żądania. Każda klasa używana w tak oflagowanej usłudze musi
mieć metodę ``setRequest``, inaczej otrzyma się błąd krytyczny::

    use Symfony\Component\HttpFoundation\Request;

    class MyClass
    {
        private $request;

        public function setRequest(Request $request)
        {
            $this->request = $request;
        }
    }

Tag cmf_published_voter
~~~~~~~~~~~~~~~~~~~~~~~

Wykorzystywany do aktywowania
:ref:`własnych wyborców <bundle-core-workflow-custom-voters>` dla
:doc:`procesu publikowania <publish_workflow>`. Oflagowanie usługi tym tagiem
integruje ``cmf_published_voter`` ją z decezją dostępu procesu publikacji.

Tag ten ma atrybut ``priority``. Im niższy numer priorytetu, tym wcześniej wyborca
będzie pobierany do głosowania.

.. _`usługą synchronizowaną`: http://symfony.com/doc/current/cookbook/service_container/scopes.html#a-using-a-synchronized-service
