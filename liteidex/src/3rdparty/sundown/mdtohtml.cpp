#include "mdtohtml.h"
#include "markdown.h"
#include "buffer.h"
#include "html.h"

#define READ_UNIT 1024
#define OUTPUT_UNIT 64

QByteArray mdtohtml(const QByteArray &data)
{
    struct buf *ob;

    struct sd_callbacks callbacks;
    struct html_renderopt options;
    struct sd_markdown *markdown;

    /* performing markdown parsing */
    ob = bufnew(OUTPUT_UNIT);

    sdhtml_renderer(&callbacks, &options, 0);
    markdown = sd_markdown_new(0, 16, &callbacks, &options);

    sd_markdown_render(ob, (uint8_t*)data.constData(), data.size(), markdown);
    sd_markdown_free(markdown);

    /* writing the result to stdout */
    //ret = fwrite(ob->data, 1, ob->size, stdout);
    QByteArray out((char*)ob->data,ob->size);
    /* cleanup */
    bufrelease(ob);

    return out;
}
