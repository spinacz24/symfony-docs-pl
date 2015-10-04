.. index::
   single: kontroler; przesyłanie plików

Jak przesyłać pliki?
====================

.. note::

    Warto rozpatrzeć zastosowanie społecznościowego pakietu `VichUploaderBundle`_,
    zamiast budowania własnej obsługi przesyłania plików. Pakiet ten dostarcza
    wszystkie popularne operacje (takie jak zmiana nazwy pliku, zapisywanie czy
    usuwanie) i jest mocno zintegrowany z Doctrine ORM, MongoDB ODM, PHPCR ODM
    oraz Propel.

Wyobraź sobie, że w aplikacji mamy encję ``Product`` i chcemy dodać broszurę PDF
dla każdego produktu. Dodajmy w tym celu w encji ``Product`` właściwość o nazwie
``brochure``::

    // src/AppBundle/Entity/Product.php
    namespace AppBundle\Entity;

    use Doctrine\ORM\Mapping as ORM;
    use Symfony\Component\Validator\Constraints as Assert;

    class Product
    {
        // ...

        /**
         * @ORM\Column(type="string")
         *
         * @Assert\NotBlank(message="Please, upload the product brochure as a PDF file.")
         * @Assert\File(mimeTypes={ "application/pdf" })
         */
        private $brochure;

        public function getBrochure()
        {
            return $this->brochure;
        }

        public function setBrochure($brochure)
        {
            $this->brochure = $brochure;

            return $this;
        }
    }

Trzeba mieć na uwadze, że typem kolumny ``brochure`` jest ``string`` a nie ``binary``
czy ``blob``, bo przechowuje ona nazwy plików PDF a nie ich zawartośc.

Następnie dodamy nowe pole ``brochure`` do formularza zarządzającego encją ``Product``::

    // src/AppBundle/Form/ProductType.php
    namespace AppBundle\Form;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class ProductType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                // ...
                ->add('brochure', 'file', array('label' => 'Brochure (PDF file)'))
                // ...
            ;
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => 'AppBundle\Entity\Product',
            ));
        }

        public function getName()
        {
            return 'product';
        }
    }

Teraz trzeba zaktualizować szablon renderujacy ten formularz tak, aby wyświetlał
nowe pole ``brochure`` (dokładnie kod szablonu dodajacy zależności w metodzie
uzywanej przez aplikację do
:doc:`dostosowania renderowania formularza </cookbook/form/form_customization>`):

.. code-block:: html+jinja

    {# app/Resources/views/product/new.html.twig #}
    <h1>Adding a new product</h1>

    {{ form_start() }}
        {# ... #}

        {{ form_row(form.brochure) }}
    {{ form_end() }}

Na koniec musimy zaktualizować kod kontrolera obsługujacego formularz::

    // src/AppBundle/Controller/ProductController.php
    namespace AppBundle\ProductController;

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\HttpFoundation\Request;
    use AppBundle\Entity\Product;
    use AppBundle\Form\ProductType;

    class ProductController extends Controller
    {
        /**
         * @Route("/product/new", name="app_product_new")
         */
        public function newAction(Request $request)
        {
            $product = new Product();
            $form = $this->createForm(new ProductType(), $product);
            $form->handleRequest($request);

            if ($form->isValid()) {
                // $file stores the uploaded PDF file
                /** @var Symfony\Component\HttpFoundation\File\UploadedFile $file */
                $file = $product->getBrochure();

                // Generate a unique name for the file before saving it
                $fileName = md5(uniqid()).'.'.$file->guessExtension();

                // Move the file to the directory where brochures are stored
                $brochuresDir = $this->container->getParameter('kernel.root_dir').'/../web/uploads/brochures';
                $file->move($brochuresDir, $fileName);

                // Update the 'brochure' property to store the PDF file name
                // instead of its contents
                $product->setBrochure($filename);

                // ... persist the $product variable or any other work

                return $this->redirect($this->generateUrl('app_product_list'));
            }

            return $this->render('product/new.html.twig', array(
                'form' => $form->createView(),
            ));
        }
    }

Jest kilka ważnych rzeczy do rozważenia w powyżej strorzonym kodzie kontrolera:

#. Gdy formularz zostanie przesłany, właściwość ``brochure`` zawiera całą zawartość
   pliku PDF. Ponieważ ta właściwość ma przechowywać tylko nazwę pliku, trzeba ustawić
   jej nową wartość zanim zostanie utrwalona zmiana encji;
#. W aplikacji Symfony, przesyłane pliki sa obiektami klasy
   :class:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile`, która
   dostarcza metody dla większości popularnych operacji wykonywanych w ramach
   przesyłania plików;
#. Uznaną praktyka bezpieczeństwa jest zasada, aby nigdy nie ufać danym wejsciowym
   dostarczanym przez użytkownikówy. Dotyczy to również plików przesyłanych przez
   odwiedzających aplikację. Klasa ``Uploaded`` dostarcza metody do pobrania
   oryginalnego rozszerzenia pliku
   (:method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::getExtension`),
   oryginalnego rozmiaru pliku
   (:method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::getClientSize`)
   i oryginalnej nazwy pliku
   (:method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::getClientOriginalName`).
   Jednak są one uważane za *niebezpieczne* ponieważ złośliwy użytkownik może
   manipulować tą informacją. Dlatego zawsze lepiej jest wygenerować unikalną nazwę
   i użyć metodę
   :method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::guessExtension`,
   pozwalając Symfony odgadnąć prawidłowe rozszerzenie na podstawie typu MIME pliku;
#. Klasa ``UploadedFile`` dostarcza również metodę
   :method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::move`
   do przechowywania pliku w wyznaczonym katalogu. Definiowanie ścieżki do tego
   katalogu jako opcji konfiguracyjnej jest uważane za dobrą praktykę, która
   upraszcza kod: ``$this->container->getParameter('brochures_dir')``.

Teraz mozna używać następującego kodu, aby utworzyć odnośnik do broszur PDF przy
każdym produkcie:

.. code-block:: html+jinja

    <a href="{{ asset('uploads/brochures/' ~ product.brochure) }}">View brochure (PDF)</a>

.. _`VichUploaderBundle`: https://github.com/dustin10/VichUploaderBundle
