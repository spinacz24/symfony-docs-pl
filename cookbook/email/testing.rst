.. index::
   testowanie poczty elektronicznej
   single: wiadomości email; testowanie
   single: poczta elektroniczna; testowanie

Jak sprawdzić w teście funkcjonalnym, czy wysyłane są wiadomości email
======================================================================

W Symfony wysyłanie wiadomości email jest bardzo proste dzięki pakietowi
SwiftmailerBundle, który wykorzystuje bibliotekę `Swift Mailer`_.

Do funkcjonalnego przetestowania działania wysyłania wiadomości email a nawet
odczytu tematu wiadomości lub innych nagłówków, można użyć
:doc:`profiler Symfony </cookbook/profiler/index>`.

Trzeba rozpocząć od jakiejś łatwej akcji kontrolera wysyłajacej wiadomość::

    public function sendEmailAction($name)
    {
        $message = \Swift_Message::newInstance()
            ->setSubject('Hello Email')
            ->setFrom('send@example.com')
            ->setTo('recipient@example.com')
            ->setBody('You should see me from the profiler!')
        ;

        $this->get('mailer')->send($message);

        return $this->render(...);
    }

.. note::

    Nie zapomnij włączyć profiler, tak jak wyjaśniono to w :doc:`/cookbook/testing/profiling`.

W teście funkcjonalnym użyj na profilerze kolektora ``swiftmailer``, aby pobrać
informację o wiadomościach wysłanych w poprzednim żądaniu::

    // src/AppBundle/Tests/Controller/MailControllerTest.php
    use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;

    class MailControllerTest extends WebTestCase
    {
        public function testMailIsSentAndContentIsOk()
        {
            $client = static::createClient();

            // Enable the profiler for the next request (it does nothing if the profiler is not available)
            $client->enableProfiler();

            $crawler = $client->request('POST', '/path/to/above/action');

            $mailCollector = $client->getProfile()->getCollector('swiftmailer');

            // Check that an email was sent
            $this->assertEquals(1, $mailCollector->getMessageCount());

            $collectedMessages = $mailCollector->getMessages();
            $message = $collectedMessages[0];

            // Asserting email data
            $this->assertInstanceOf('Swift_Message', $message);
            $this->assertEquals('Hello Email', $message->getSubject());
            $this->assertEquals('send@example.com', key($message->getFrom()));
            $this->assertEquals('recipient@example.com', key($message->getTo()));
            $this->assertEquals(
                'You should see me from the profiler!',
                $message->getBody()
            );
        }
    }

.. _`Swift Mailer`: http://swiftmailer.org/
