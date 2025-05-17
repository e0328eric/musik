use std::fs::File;
use std::io::BufReader;

use rodio::{source::Source, Decoder, OutputStream, Sink};

fn main() -> anyhow::Result<()> {
    let (_stream, stream_handle) = OutputStream::try_default()?;
    let sink = Sink::try_new(&stream_handle)?;
    let file = BufReader::new(File::open("misty_rainbow.flac")?);
    let source = Decoder::new(file)?;

    sink.append(source);
    sink.sleep_until_end();

    Ok(())
}
