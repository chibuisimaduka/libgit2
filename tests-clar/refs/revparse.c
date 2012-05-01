#include "clar_libgit2.h"

#include "git2/revparse.h"

static git_repository *g_repo;
static git_object *g_obj;



/* Helpers */
static void oid_str_cmp(const git_object *obj, const char *expected)
{
   char objstr[64] = {0};
   git_oid_to_string(objstr, 64, git_object_id(obj));
   cl_assert_equal_s(objstr, expected);
}


void test_refs_revparse__initialize(void)
{
   g_repo = cl_git_sandbox_init("testrepo.git");
}

void test_refs_revparse__cleanup(void)
{
   cl_git_sandbox_cleanup();
   g_obj = NULL;
}


void test_refs_revparse__nonexistant_object(void)
{
   cl_git_fail(git_revparse_single(&g_obj, g_repo, "this doesn't exist"));
}

void test_refs_revparse__shas(void)
{
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "c47800c7266a2be04c571c04d5a6614691ea99bd"));
   oid_str_cmp(g_obj, "c47800c7266a2be04c571c04d5a6614691ea99bd");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "c47800c"));
   oid_str_cmp(g_obj, "c47800c7266a2be04c571c04d5a6614691ea99bd");
}

void test_refs_revparse__head(void)
{
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "HEAD"));
   oid_str_cmp(g_obj, "a65fedf39aefe402d3bb6e24df4d4f5fe4547750");
}

void test_refs_revparse__full_refs(void)
{
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "refs/heads/master"));
   oid_str_cmp(g_obj, "a65fedf39aefe402d3bb6e24df4d4f5fe4547750");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "refs/heads/test"));
   oid_str_cmp(g_obj, "e90810b8df3e80c413d903f631643c716887138d");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "refs/tags/test"));
   oid_str_cmp(g_obj, "b25fa35b38051e4ae45d4222e795f9df2e43f1d1");
}

void test_refs_revparse__partial_refs(void)
{
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "point_to_blob"));
   oid_str_cmp(g_obj, "1385f264afb75a56a5bec74243be9b367ba4ca08");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "packed-test"));
   oid_str_cmp(g_obj, "4a202b346bb0fb0db7eff3cffeb3c70babbd2045");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "br2"));
   oid_str_cmp(g_obj, "a4a7dce85cf63874e984719f4fdd239f5145052f");
}

void test_refs_revparse__describe_output(void)
{
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "blah-7-gc47800c"));
   oid_str_cmp(g_obj, "c47800c7266a2be04c571c04d5a6614691ea99bd");
}

void test_refs_revparse__nth_parent(void)
{
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "be3563a^1"));
   oid_str_cmp(g_obj, "9fd738e8f7967c078dceed8190330fc8648ee56a");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "be3563a^"));
   oid_str_cmp(g_obj, "9fd738e8f7967c078dceed8190330fc8648ee56a");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "be3563a^2"));
   oid_str_cmp(g_obj, "c47800c7266a2be04c571c04d5a6614691ea99bd");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "be3563a^1^1"));
   oid_str_cmp(g_obj, "4a202b346bb0fb0db7eff3cffeb3c70babbd2045");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "be3563a^2^1"));
   oid_str_cmp(g_obj, "5b5b025afb0b4c913b4c338a42934a3863bf3644");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "be3563a^0"));
   oid_str_cmp(g_obj, "be3563ae3f795b2b4353bcce3a527ad0a4f7f644");
}

void test_refs_revparse__not_tag(void)
{
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "point_to_blob^{}"));
   oid_str_cmp(g_obj, "1385f264afb75a56a5bec74243be9b367ba4ca08");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "wrapped_tag^{}"));
   oid_str_cmp(g_obj, "a65fedf39aefe402d3bb6e24df4d4f5fe4547750");
}

void test_refs_revparse__to_type(void)
{
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "wrapped_tag^{commit}"));
   oid_str_cmp(g_obj, "a65fedf39aefe402d3bb6e24df4d4f5fe4547750");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "wrapped_tag^{tree}"));
   oid_str_cmp(g_obj, "944c0f6e4dfa41595e6eb3ceecdb14f50fe18162");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "point_to_blob^{blob}"));
   oid_str_cmp(g_obj, "1385f264afb75a56a5bec74243be9b367ba4ca08");

   cl_git_fail(git_revparse_single(&g_obj, g_repo, "wrapped_tag^{blob}"));
}

void test_refs_revparse__linear_history(void)
{
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "master~0"));
   oid_str_cmp(g_obj, "a65fedf39aefe402d3bb6e24df4d4f5fe4547750");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "master~1"));
   oid_str_cmp(g_obj, "be3563ae3f795b2b4353bcce3a527ad0a4f7f644");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "master~2"));
   oid_str_cmp(g_obj, "9fd738e8f7967c078dceed8190330fc8648ee56a");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "master~1~1"));
   oid_str_cmp(g_obj, "9fd738e8f7967c078dceed8190330fc8648ee56a");
}

void test_refs_revparse__chaining(void)
{
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "master~1^1"));
   oid_str_cmp(g_obj, "9fd738e8f7967c078dceed8190330fc8648ee56a");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "master~1^2"));
   oid_str_cmp(g_obj, "c47800c7266a2be04c571c04d5a6614691ea99bd");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "master^1^2~1"));
   oid_str_cmp(g_obj, "5b5b025afb0b4c913b4c338a42934a3863bf3644");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "master^1^1^1^1^1"));
   oid_str_cmp(g_obj, "8496071c1b46c854b31185ea97743be6a8774479");
}

void test_refs_revparse__reflog(void)
{
   cl_git_fail(git_revparse_single(&g_obj, g_repo, "@{-xyz}"));
   cl_git_fail(git_revparse_single(&g_obj, g_repo, "@{-0}"));

   cl_git_pass(git_revparse_single(&g_obj, g_repo, "@{-2}"));
   oid_str_cmp(g_obj, "a65fedf39aefe402d3bb6e24df4d4f5fe4547750");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "@{-1}"));
   oid_str_cmp(g_obj, "a4a7dce85cf63874e984719f4fdd239f5145052f");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "master@{0}"));
   oid_str_cmp(g_obj, "a65fedf39aefe402d3bb6e24df4d4f5fe4547750");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "master@{1}"));
   oid_str_cmp(g_obj, "be3563ae3f795b2b4353bcce3a527ad0a4f7f644");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "@{0}"));
   oid_str_cmp(g_obj, "a65fedf39aefe402d3bb6e24df4d4f5fe4547750");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "@{1}"));
   oid_str_cmp(g_obj, "be3563ae3f795b2b4353bcce3a527ad0a4f7f644");
   /* Not ready yet
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "HEAD@{100 years ago}"));
   oid_str_cmp(g_obj, "a65fedf39aefe402d3bb6e24df4d4f5fe4547750");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "master@{2012-4-30 10:23:20}"));
   oid_str_cmp(g_obj, "be3563ae3f795b2b4353bcce3a527ad0a4f7f644");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "master@{upstream}"));
   oid_str_cmp(g_obj, "???");
   cl_git_pass(git_revparse_single(&g_obj, g_repo, "master@{u}"));
   oid_str_cmp(g_obj, "???");
   */
}
